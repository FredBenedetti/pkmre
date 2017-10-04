#ifndef GUARD_FLAGS_H
#define GUARD_FLAGS_H

#define TRAINER_FLAG_START  0x500
#define TRAINERS_FLAG_NO    0x356
#define CODE_FLAGS          (TRAINER_FLAG_START + TRAINERS_FLAG_NO + 0xA) // 0x860

// SYSTEM FLAGS

#define SYS_POKEMON_GET     CODE_FLAGS + 0
#define SYS_POKEDEX_GET     CODE_FLAGS + 1
#define SYS_POKENAV_GET     CODE_FLAGS + 2
// third one appears unused
#define SYS_GAME_CLEAR      CODE_FLAGS + 4
#define SYS_CHAT_USED       CODE_FLAGS + 5
#define SYS_HIPSTER_MEET    CODE_FLAGS + 6

// badges
#define BADGE01_GET         CODE_FLAGS + 7
#define BADGE02_GET         CODE_FLAGS + 8
#define BADGE03_GET         CODE_FLAGS + 9
#define BADGE04_GET         CODE_FLAGS + 0xA
#define BADGE05_GET         CODE_FLAGS + 0xB
#define BADGE06_GET         CODE_FLAGS + 0xC
#define BADGE07_GET         CODE_FLAGS + 0xD
#define BADGE08_GET         CODE_FLAGS + 0xE

// cities and towns
#define FLAG_VISITED_LITTLEROOT_TOWN  CODE_FLAGS + 0xF
#define FLAG_VISITED_OLDALE_TOWN      CODE_FLAGS + 0x10
#define FLAG_VISITED_DEWFORD_TOWN     CODE_FLAGS + 0x11
#define FLAG_VISITED_LAVARIDGE_TOWN   CODE_FLAGS + 0x12
#define FLAG_VISITED_FALLARBOR_TOWN   CODE_FLAGS + 0x13
#define FLAG_VISITED_VERDANTURF_TOWN  CODE_FLAGS + 0x14
#define FLAG_VISITED_PACIFIDLOG_TOWN  CODE_FLAGS + 0x15
#define FLAG_VISITED_PETALBURG_CITY   CODE_FLAGS + 0x16
#define FLAG_VISITED_SLATEPORT_CITY   CODE_FLAGS + 0x17
#define FLAG_VISITED_MAUVILLE_CITY    CODE_FLAGS + 0x18
#define FLAG_VISITED_RUSTBORO_CITY    CODE_FLAGS + 0x19
#define FLAG_VISITED_FORTREE_CITY     CODE_FLAGS + 0x1A
#define FLAG_VISITED_LILYCOVE_CITY    CODE_FLAGS + 0x1B
#define FLAG_VISITED_MOSSDEEP_CITY    CODE_FLAGS + 0x1C
#define FLAG_VISITED_SOOTOPOLIS_CITY  CODE_FLAGS + 0x1D
#define FLAG_VISITED_EVER_GRANDE_CITY CODE_FLAGS + 0x1E

#define SYS_UNKNOWN_880      CODE_FLAGS + 0x20

#define SYS_USE_FLASH        CODE_FLAGS + 0x28
#define SYS_USE_STRENGTH     CODE_FLAGS + 0x29
#define SYS_WEATHER_CTRL     CODE_FLAGS + 0x2A
#define SYS_CYCLING_ROAD     CODE_FLAGS + 0x2B
#define SYS_SAFARI_MODE      CODE_FLAGS + 0x2C
#define SYS_CRUISE_MODE      CODE_FLAGS + 0x2D

#define SYS_TV_HOME          CODE_FLAGS + 0x30
#define SYS_TV_WATCH         CODE_FLAGS + 0x31
#define SYS_TV_START         CODE_FLAGS + 0x32
#define SYS_POPWORD_INPUT    CODE_FLAGS + 0x33
#define SYS_MIX_RECORD       CODE_FLAGS + 0x34
#define SYS_CLOCK_SET        CODE_FLAGS + 0x35
#define SYS_NATIONAL_DEX     CODE_FLAGS + 0x36
#define SYS_CAVE_SHIP        CODE_FLAGS + 0x37
#define SYS_CAVE_WONDER      CODE_FLAGS + 0x38
#define SYS_CAVE_BATTLE      CODE_FLAGS + 0x39
#define SYS_SHOAL_TIDE       CODE_FLAGS + 0x3A
#define SYS_RIBBON_GET       CODE_FLAGS + 0x3B

#define SYS_PC_LANETTE              CODE_FLAGS + 0x4B
#define SYS_MYSTERY_EVENT_ENABLE    CODE_FLAGS + 0x4C
#define SYS_ENC_UP_ITEM             CODE_FLAGS + 0x4D
#define SYS_ENC_DOWN_ITEM           CODE_FLAGS + 0x4E
#define SYS_BRAILLE_DIG             CODE_FLAGS + 0x4F
#define SYS_BRAILLE_STRENGTH        CODE_FLAGS + 0x50
#define SYS_BRAILLE_WAIT            CODE_FLAGS + 0x51
#define SYS_BRAILLE_FLY             CODE_FLAGS + 0x52
#define SYS_HAS_EON_TICKET          CODE_FLAGS + 0x53

#define SYS_POKEMON_LEAGUE_FLY CODE_FLAGS + 0x54

#define SYS_TV_LATI          CODE_FLAGS + 0x5D

#define SYS_SHOAL_ITEM       CODE_FLAGS + 0x5F
#define SYS_B_DASH           CODE_FLAGS + 0x60 // got Running Shoes
#define SYS_CTRL_OBJ_DELETE  CODE_FLAGS + 0x61
#define SYS_RESET_RTC_ENABLE CODE_FLAGS + 0x62

#define SYS_STORAGE_UNKNOWN_FLAG    CODE_FLAGS + 0x77

#define SYS_MYSTERY_GIFT_ENABLE     CODE_FLAGS + 0x7B

// SPECIAL FLAGS (unknown purpose)

#define SPECIAL_FLAG_1 0x4001

#endif // GUARD_FLAGS_H
