#include "config.h"
#include "bsp_komihash.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ws2812.h"
#include "pico/unique_id.h"
#include "pico/rand.h"
#include "eeprom.h"
#include "base64.h"
#include "common.h"
#define KOMISEED    0x50726f66506c756d

typedef struct _config_t {
    char handle[15];
    char class;
    uint32_t achievements;
    uint16_t totalscore;
    uint8_t votes;
    uint8_t missions;   // completed missions, persists across runs (same record offset as the old unused byte)
    uint64_t hash;
} config_t;

config_t g_config={0};
char* achieve_str = NULL;
char my_id[13] = {0};

uint64_t achievement_hashes[32] = {
    0,                     //                       - Konami code
    0,                     //                       - Complete one mission
    0xa60d149ba625229fULL, // XX SC_VECTOR_BURN        - 9000 points
    0x2e0bf85ad010dbb2ULL, // XX SC_NOVA_TRACE         - In game
    0x4750921fb1958f66ULL, // XX SC_ION_WAKE           - In game
    0x666d527fb94e3da0ULL, // XX SC_SOLAR_SHEAR        - full crew
    0x80a5aedb8684c3b4ULL, // XX SC_PLASMA_ARC         - unique crew
    0x54cd46567937ecbdULL, // XX SC_VOID_VECTOR        - social 1
    0xc327e386d9189b63ULL, // XX SC_STAR_OFFSET        - social 2
    0xd1ff3ce39c469e68ULL, // XX SC_ORBIT_DECAY        - voting 
    0x3c317b15e7c1e29dULL, // SC_THRUST_LOSS        - RE flash. 
    0xa1d4a114f76211b7ULL, // SC_SIGNAL_GHOST       - EEPROM
    0x95c911db5174adaeULL, // SC_ECHO_ORBIT         - Offline mode
    0x236c4a02f5b53468ULL, // SC_DUST_FLUX          - USB UART
    0xca5d48f6551e91a6ULL, // SC_PHASE_DRIFT        - UART DOCK
    0x3b21dd157c302499ULL, // SC_GRAV_SHIFT         - Crypto
    0x2b649143bd4140b1ULL, // SC_CORE_FLUX          - Boot 
    0x73d4fbc7ac29b3ffULL, // SC_LAGRANGE_LOCK      - Badge Art 5 bit
    0x187b1fcf223c7378ULL, // SC_PULSE_FIELD        - Badge art Contelations
    0x1d3ba22e7f078e03ULL, // SC_FIELD_ECHO         - Committee
    0x5d0f1b55a201937eULL, // SC_QUANTUM_WAKE       - AI Community
    0x3260bcce868c358cULL, // SC_DEEP_PARALLAX      - AppSec
    0xa3ffa1442b62a8b9ULL, // SC_RADIAL_DRIFT       - Bitstream
    0xfd4dcde6773030f5ULL, // SC_COSMIC_BIAS        - Blue Team
    0x72b59e0887bae55cULL, // SC_FLUX_VECTOR        - Hardware hacking
    0xbd21f31f1860e478ULL, // SC_ORBIT_TRACE        - Healthcare hacking
    0x5bb3c9f2243bcbddULL, // SC_ASTRA_SHIFT        - Homelabs
    0x36ee9016f26b3c8dULL, // SC_ZENITH_DRIFT       - ID/IOT
    0x70dd514041fafbf3ULL, // SC_ECHO_VECTOR        - Lockpicking
    0xb377e54b5a08a833ULL, // SC_DRIFT_LOCK         - Privacy
    0x6c01b05f902d9e09ULL, // SC_APOGEE_DRIFT       - Tamper evident
    0xfccc75461d4f2fe2ULL, // SC_PERIGEE_SHIFT      - The keep
};

char *achievement_names[32] = {
    "Thirty Lives", 
    "First Departure",
    "Frequent Flyer",
    "Relics of the First Builders",
    "The Obsidian Monolith",
    "Full Complement",
    "A Crew of Specialists",
    "Signed On",
    "Works Well With Others",
    "Civic Duty",
    "Deep Storage",
    "Long-Term Memory",
    "Manual Override",
    "Service Port",
    "Dockmaster's Channel",
    "Misaligned Home Row",
    "Diagnostics Complete",
    "Circles Within Circles",
    "Connect the Dots",
    "Know a Guy",
    "Synthetic Minds",          //AI
    "Attack Surface Survey",    //AppSec
    "Fabric of Reality",        //Bitstream
    "Blue Skies Ahead",         //Blue Team
    "Bare Metal",               //Hardware hacking
    "Sick Bay",                 //Healthcare hacking
    "Domestic Infrastructure",  //Homelabs
    "Identity Crisis",          //ID/IOT
    "Open Access",              //Lock picking
    "Off the Record",           //Privacy 
    "Seal Broken",              //Tamper evident
    "The Watchtower",           //The keep
    "Access Granted",           //RFID lab
};

void config_set_id() {
    pico_unique_board_id_t id;
    pico_get_unique_board_id (&id);
    base64_encode((uint8_t*)&id, sizeof(id), my_id, sizeof(my_id));
    my_id[5] = '\0';
}

int config_load() {
    config_set_id();
    memset(&g_config, 0, sizeof(g_config));
    int ret = eeprom_readbytes(CONFIG_ADDR, (uint8_t*)&g_config, sizeof(g_config));
    uint64_t hash = komihash(&g_config, sizeof(g_config)-8, KOMISEED);
    return hash == g_config.hash;
}

void config_write() {
    g_config.hash = komihash(&g_config, sizeof(g_config)-8, KOMISEED);
    int ret = eeprom_writebytes(CONFIG_ADDR, (uint8_t*)&g_config, sizeof(g_config));
}

void config_clear() {
    char zeros[64] = {0};
    for (int i=0; i<EEPROM_SIZE_BYTES; i+=64) {
        eeprom_writebytes(i, zeros, sizeof(zeros));
    }
    memset(&g_config, 0, sizeof(g_config));
    g_config.class = 'A' + (get_rand_32() %5);
    config_generate_handle();
    config_write();
    volatile char eeprom_unlock[] = {6, 22, 10, 6, 28, 18, 27, 20, 25, 10, 18, 29, 26, 6, 1, 0x55}; //SC_SIGNAL_GHOST
    for (int i=0; i< 16; ++i)
        eeprom_unlock[i]^= 0x55;
    eeprom_writebytes(0, eeprom_unlock, sizeof(eeprom_unlock));
}

void config_set_handle(const char* name) {
    snprintf(g_config.handle, sizeof(g_config.handle), "%s", name);
    config_write();
}

int config_get_score() {
    return g_config.totalscore;
}

int config_get_missions() {
    return g_config.missions & 0x3f;
}

int config_get_perks() {
    return g_config.missions >> 6;   // two perk bits share the byte with the mission count
}

void config_add_perk(int bit) {
    if (g_config.missions & (bit << 6)) return;
    g_config.missions |= (bit & 3) << 6;
    config_write();
}

void config_add_mission(int score) {
    if ((g_config.missions & 0x3f) < 63) g_config.missions++;
    uint32_t t = (uint32_t)g_config.totalscore + (uint32_t)(score > 0 ? score : 0);
    g_config.totalscore = t > 65535 ? 65535 : (uint16_t)t;
    config_write();
}

int config_get_achievement_count() {
    int count = 0;
    for (int i=0; i<32; ++i) {
        if (g_config.achievements & (1<<i)) {
            count++;
        }
    }
    return count;
}

uint32_t config_get_achievements() {
    return g_config.achievements;
}

char* config_get_handle() {
    return g_config.handle;
}

char config_get_class() {
    return g_config.class;
}

char* get_class_name(char c) {
    switch (c) {
        case CREW_SPECIALTY_PILOT: return "Pilot";
        case CREW_SPECIALTY_ENGINEER: return "Engineer";
        case CREW_SPECIALTY_SCIENTIST: return "Scientist";
        case CREW_SPECIALTY_SOLDIER: return "Soldier";
        case CREW_SPECIALTY_MEDIC: return "Medic";
        default: return "Unknown";
    }
}

static const char* pilot_name_pool_1[50] = {
    "Aerion", "Blaze", "Cypher", "Drake", "Eclipt", "Falco", "Galaxy", "Hawke",
    "Ignite", "Jetson", "Krypta", "Lunar", "Matrix", "Nexus", "Oracle", "Pulsar",
    "Quasar", "Ranger", "Sirius", "Titan", "Unison", "Vector", "Warped", "Xenith",
    "Yonder", "Zealot", "Astra", "Brave", "Corvus", "Deltar", "Elixir", "Forge",
    "Gravus", "Helion", "Inert", "Junoxy", "Kalibr", "Lumina", "Mirage", "Noctis",
    "Origen", "Photon", "Quinox", "Rey", "Stell", "Talion", "Usher", "Vortex",
    "Wraith", "Xycor"
};

static const char* pilot_name_pool_2[50] = {
    "Arion", "Bolter", "Citro", "Dexter", "Ensign", "Fusion", "Gambit", "Horiz",
    "Inert", "Javel", "Kernel", "Legion", "Mantis", "Nimbus", "Omnia", "Phonyx",
    "Quanta", "Raptor", "Scion", "Talon", "Uplink", "Vulcan", "Widgit", "Xyste",
    "Yale", "Zenith", "Arcade", "Binary", "Cirrus", "Dynamo", "Echo", "Flux",
    "Grado", "Hunter", "Illume", "Joule", "Kalmix", "Lance", "Novia", "Orbit",
    "Pandor", "Quorum", "Radius", "Strata", "Torque", "Umbrae", "Vigor", "Welkin",
    "Xeno", "Zepra"
};

void config_generate_handle(void) {
    const char* first = pilot_name_pool_1[get_rand_32() % 50];
    const char* second = pilot_name_pool_2[get_rand_32() % 50];
    
    memset(g_config.handle, 0, sizeof(g_config.handle));
    snprintf(g_config.handle, sizeof(g_config.handle), "%s %s", first, second);
}

int config_submit(const char* key) {
    uint64_t hash = komihash(key, strlen(key), KOMISEED);
    for (int i=2; i<32; ++i) {
        if (hash == achievement_hashes[i]) {
            return config_achievement(i);
        }
    }
    ws2812_show_alert((ws2812_rgb_t){32,0,0});
    return -1;
}

int config_achievement(enum achievement id) {
    if (g_config.achievements & (1<<id)) {
        return 0;
    }
    else {
        g_config.achievements |= (1<<id);
        config_write();
        achieve_str = achievement_names[id];
        ws2812_show_alert((ws2812_rgb_t){32,0,32});
        return 1;
    }
}

char* config_get_achievement_name(enum achievement id) {
    if (id < 0 || id >= 32) return "Invalid Achievement";
    return achievement_names[id];
}

char* config_get_id() { 
    return my_id;
}

void config_add_vote() {
    g_config.votes++;
    config_write();
    if (g_config.votes >= 3) {
        config_achievement(voter);
    }
}