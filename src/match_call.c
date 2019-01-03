#include "global.h"
#include "alloc.h"
#include "battle.h"
#include "battle_setup.h"
#include "bg.h"
#include "data2.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "main.h"
#include "menu.h"
#include "new_game.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "region_map.h"
#include "rtc.h"
#include "script.h"
#include "script_movement.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "wild_encounter.h"
#include "window.h"
#include "constants/abilities.h"
#include "constants/battle_frontier.h"
#include "constants/event_objects.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/trainers.h"

struct MatchCallState
{
    u32 minutes;
    u16 trainerId;
    u8 stepCounter;
    u8 triggeredFromScript;
};

struct MatchCallTrainerTextInfo
{
    u16 trainerId;
    u16 unused;
    u16 battleTopicTextIds[3];
    u16 battleFrontierStreakTextId;
    u8 battleFrontierRecordStreakTextIndex;
    u16 sameRouteMatchCallTextId;
    u16 differentRouteMatchCallTextId;
};

struct MatchCallText
{
    const u8 *text;
    s8 stringVarFuncIds[3];
};

struct MultiTrainerMatchCallText
{
    u16 trainerId;
    const u8 *text;
};

struct BattleFrontierStreakInfo
{
    u16 facilityId;
    u16 streak;
};

EWRAM_DATA struct MatchCallState gMatchCallState = {0};
EWRAM_DATA struct BattleFrontierStreakInfo gBattleFrontierStreakInfo = {0};

bool32 SelectMatchCallMessage(int, u8 *);
static u32 GetCurrentTotalMinutes(struct Time *);
static u32 GetNumRegisteredNPCs(void);
static u32 GetActiveMatchCallTrainerId(u32);
static int GetTrainerMatchCallId(int);
static u16 GetRematchTrainerLocation(int);
static bool32 TrainerIsEligibleForRematch(int);
static void StartMatchCall(void);
static void ExecuteMatchCall(u8);
static void DrawMatchCallTextBoxBorder(u32, u32, u32);
static void sub_8196694(u8);
static void InitMatchCallTextPrinter(int, const u8 *);
static bool32 ExecuteMatchCallTextPrinter(int);
static const struct MatchCallText *GetSameRouteMatchCallText(int, u8 *);
static const struct MatchCallText *GetDifferentRouteMatchCallText(int, u8 *);
static const struct MatchCallText *GetBattleMatchCallText(int, u8 *);
static const struct MatchCallText *GetGeneralMatchCallText(int, u8 *);
static bool32 sub_8196D74(int);
static void BuildMatchCallString(int, const struct MatchCallText *, u8 *);
static u16 GetFrontierStreakInfo(u16, u32 *);
static void PopulateMatchCallStringVars(int, const s8 *);
static void PopulateMatchCallStringVar(int, int, u8 *);
static bool32 LoadMatchCallWindowGfx(u8);
static bool32 MoveMatchCallWindowToVram(u8);
static bool32 PrintMatchCallIntroEllipsis(u8);
static bool32 sub_81962B0(u8);
static bool32 sub_81962D8(u8);
static bool32 sub_8196330(u8);
static bool32 sub_8196390(u8);
static bool32 sub_81963F0(u8);
static void PopulateTrainerName(int, u8 *);
static void PopulateMapName(int, u8 *);
static void PopulateSpeciesFromTrainerLocation(int, u8 *);
static void PopulateSpeciesFromTrainerParty(int, u8 *);
static void PopulateBattleFrontierFacilityName(int, u8 *);
static void PopulateBattleFrontierStreak(int, u8 *);

extern void sub_81973A4(void);

#define TEXT_ID(topic, id) (((topic) << 8) | ((id) & 0xFF))

static const struct MatchCallTrainerTextInfo sMatchCallTrainers[] =
{
    {
        .trainerId = TRAINER_ROSE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 3),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_ANDRES_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 12), TEXT_ID(2, 12), TEXT_ID(3, 12) },
        .battleFrontierStreakTextId = TEXT_ID(1, 62),
        .battleFrontierRecordStreakTextIndex = 12,
        .sameRouteMatchCallTextId = TEXT_ID(1, 12),
        .differentRouteMatchCallTextId = TEXT_ID(2, 12),
    },
    {
        .trainerId = TRAINER_DUSTY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 12), TEXT_ID(2, 12), TEXT_ID(3, 12) },
        .battleFrontierStreakTextId = TEXT_ID(1, 4),
        .battleFrontierRecordStreakTextIndex = 12,
        .sameRouteMatchCallTextId = TEXT_ID(1, 12),
        .differentRouteMatchCallTextId = TEXT_ID(2, 12),
    },
    {
        .trainerId = TRAINER_LOLA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 2), TEXT_ID(2, 2), TEXT_ID(3, 2) },
        .battleFrontierStreakTextId = TEXT_ID(1, 5),
        .battleFrontierRecordStreakTextIndex = 2,
        .sameRouteMatchCallTextId = TEXT_ID(1, 2),
        .differentRouteMatchCallTextId = TEXT_ID(2, 2),
    },
    {
        .trainerId = TRAINER_RICKY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 6),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_LILA_AND_ROY_1,
        .unused = 4,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 61),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_CRISTIN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 10), TEXT_ID(2, 10), TEXT_ID(3, 10) },
        .battleFrontierStreakTextId = TEXT_ID(1, 64),
        .battleFrontierRecordStreakTextIndex = 10,
        .sameRouteMatchCallTextId = TEXT_ID(1, 10),
        .differentRouteMatchCallTextId = TEXT_ID(2, 10),
    },
    {
        .trainerId = TRAINER_BROOKE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 8),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_WILTON_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 6), TEXT_ID(2, 6), TEXT_ID(3, 6) },
        .battleFrontierStreakTextId = TEXT_ID(1, 7),
        .battleFrontierRecordStreakTextIndex = 6,
        .sameRouteMatchCallTextId = TEXT_ID(1, 6),
        .differentRouteMatchCallTextId = TEXT_ID(2, 6),
    },
    {
        .trainerId = TRAINER_VALERIE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 9),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_CINDY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 10),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_THALIA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 10), TEXT_ID(3, 10) },
        .battleFrontierStreakTextId = TEXT_ID(1, 14),
        .battleFrontierRecordStreakTextIndex = 10,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 10),
    },
    {
        .trainerId = TRAINER_JESSICA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 10), TEXT_ID(2, 10), TEXT_ID(3, 10) },
        .battleFrontierStreakTextId = TEXT_ID(1, 11),
        .battleFrontierRecordStreakTextIndex = 10,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 10),
    },
    {
        .trainerId = TRAINER_WINSTON_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 4), TEXT_ID(2, 4), TEXT_ID(3, 4) },
        .battleFrontierStreakTextId = TEXT_ID(1, 12),
        .battleFrontierRecordStreakTextIndex = 4,
        .sameRouteMatchCallTextId = TEXT_ID(1, 4),
        .differentRouteMatchCallTextId = TEXT_ID(2, 4),
    },
    {
        .trainerId = TRAINER_STEVE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 7), TEXT_ID(2, 7), TEXT_ID(3, 7) },
        .battleFrontierStreakTextId = TEXT_ID(1, 13),
        .battleFrontierRecordStreakTextIndex = 7,
        .sameRouteMatchCallTextId = TEXT_ID(1, 7),
        .differentRouteMatchCallTextId = TEXT_ID(2, 7),
    },
    {
        .trainerId = TRAINER_TONY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 15),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_NOB_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 16),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_KOJI_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 59),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_FERNANDO_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 6), TEXT_ID(2, 6), TEXT_ID(3, 6) },
        .battleFrontierStreakTextId = TEXT_ID(1, 17),
        .battleFrontierRecordStreakTextIndex = 6,
        .sameRouteMatchCallTextId = TEXT_ID(1, 6),
        .differentRouteMatchCallTextId = TEXT_ID(2, 6),
    },
    {
        .trainerId = TRAINER_DALTON_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 4), TEXT_ID(2, 4), TEXT_ID(3, 4) },
        .battleFrontierStreakTextId = TEXT_ID(1, 18),
        .battleFrontierRecordStreakTextIndex = 4,
        .sameRouteMatchCallTextId = TEXT_ID(1, 4),
        .differentRouteMatchCallTextId = TEXT_ID(2, 4),
    },
    {
        .trainerId = TRAINER_BERNIE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 11), TEXT_ID(2, 11), TEXT_ID(3, 11) },
        .battleFrontierStreakTextId = TEXT_ID(1, 19),
        .battleFrontierRecordStreakTextIndex = 11,
        .sameRouteMatchCallTextId = TEXT_ID(1, 11),
        .differentRouteMatchCallTextId = TEXT_ID(2, 11),
    },
    {
        .trainerId = TRAINER_ETHAN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 20),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_JOHN_AND_JAY_1,
        .unused = 3,
        .battleTopicTextIds = { TEXT_ID(1, 12), TEXT_ID(2, 12), TEXT_ID(3, 12) },
        .battleFrontierStreakTextId = TEXT_ID(1, 60),
        .battleFrontierRecordStreakTextIndex = 12,
        .sameRouteMatchCallTextId = TEXT_ID(1, 12),
        .differentRouteMatchCallTextId = TEXT_ID(2, 12),
    },
    {
        .trainerId = TRAINER_JEFFREY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 7), TEXT_ID(2, 7), TEXT_ID(3, 7) },
        .battleFrontierStreakTextId = TEXT_ID(1, 21),
        .battleFrontierRecordStreakTextIndex = 7,
        .sameRouteMatchCallTextId = TEXT_ID(1, 7),
        .differentRouteMatchCallTextId = TEXT_ID(2, 7),
    },
    {
        .trainerId = TRAINER_CAMERON_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 4), TEXT_ID(2, 4), TEXT_ID(3, 4) },
        .battleFrontierStreakTextId = TEXT_ID(1, 22),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 4),
        .differentRouteMatchCallTextId = TEXT_ID(2, 4),
    },
    {
        .trainerId = TRAINER_JACKI_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 23),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_WALTER_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 12), TEXT_ID(2, 12), TEXT_ID(3, 12) },
        .battleFrontierStreakTextId = TEXT_ID(1, 24),
        .battleFrontierRecordStreakTextIndex = 12,
        .sameRouteMatchCallTextId = TEXT_ID(1, 12),
        .differentRouteMatchCallTextId = TEXT_ID(2, 12),
    },
    {
        .trainerId = TRAINER_KAREN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 2), TEXT_ID(2, 2), TEXT_ID(3, 2) },
        .battleFrontierStreakTextId = TEXT_ID(1, 26),
        .battleFrontierRecordStreakTextIndex = 2,
        .sameRouteMatchCallTextId = TEXT_ID(1, 2),
        .differentRouteMatchCallTextId = TEXT_ID(2, 2),
    },
    {
        .trainerId = TRAINER_JERRY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 25),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_ANNA_AND_MEG_1,
        .unused = 6,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 27),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_ISABEL_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 14), TEXT_ID(2, 14), TEXT_ID(3, 14) },
        .battleFrontierStreakTextId = TEXT_ID(1, 29),
        .battleFrontierRecordStreakTextIndex = 14,
        .sameRouteMatchCallTextId = TEXT_ID(1, 14),
        .differentRouteMatchCallTextId = TEXT_ID(2, 14),
    },
    {
        .trainerId = TRAINER_MIGUEL_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 11), TEXT_ID(2, 11), TEXT_ID(3, 11) },
        .battleFrontierStreakTextId = TEXT_ID(1, 28),
        .battleFrontierRecordStreakTextIndex = 11,
        .sameRouteMatchCallTextId = TEXT_ID(1, 11),
        .differentRouteMatchCallTextId = TEXT_ID(2, 11),
    },
    {
        .trainerId = TRAINER_TIMOTHY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 12), TEXT_ID(2, 12), TEXT_ID(3, 12) },
        .battleFrontierStreakTextId = TEXT_ID(1, 30),
        .battleFrontierRecordStreakTextIndex = 12,
        .sameRouteMatchCallTextId = TEXT_ID(1, 12),
        .differentRouteMatchCallTextId = TEXT_ID(2, 12),
    },
    {
        .trainerId = TRAINER_SHELBY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 13), TEXT_ID(2, 13), TEXT_ID(3, 13) },
        .battleFrontierStreakTextId = TEXT_ID(1, 31),
        .battleFrontierRecordStreakTextIndex = 13,
        .sameRouteMatchCallTextId = TEXT_ID(1, 13),
        .differentRouteMatchCallTextId = TEXT_ID(2, 13),
    },
    {
        .trainerId = TRAINER_CALVIN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 32),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_ELLIOT_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 33),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_ISAIAH_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 38),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_MARIA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 37),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_ABIGAIL_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 35),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_DYLAN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 36),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_KATELYN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 40),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_BENJAMIN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 34),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_PABLO_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 39),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_NICOLAS_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 4), TEXT_ID(2, 4), TEXT_ID(3, 4) },
        .battleFrontierStreakTextId = TEXT_ID(1, 41),
        .battleFrontierRecordStreakTextIndex = 4,
        .sameRouteMatchCallTextId = TEXT_ID(1, 4),
        .differentRouteMatchCallTextId = TEXT_ID(2, 4),
    },
    {
        .trainerId = TRAINER_ROBERT_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 6), TEXT_ID(2, 6), TEXT_ID(3, 6) },
        .battleFrontierStreakTextId = TEXT_ID(1, 42),
        .battleFrontierRecordStreakTextIndex = 6,
        .sameRouteMatchCallTextId = TEXT_ID(1, 6),
        .differentRouteMatchCallTextId = TEXT_ID(2, 6),
    },
    {
        .trainerId = TRAINER_LAO_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 43),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_CYNDY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 44),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_MADELINE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 45),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_JENNY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 46),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_DIANA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 2), TEXT_ID(2, 2), TEXT_ID(3, 2) },
        .battleFrontierStreakTextId = TEXT_ID(1, 47),
        .battleFrontierRecordStreakTextIndex = 2,
        .sameRouteMatchCallTextId = TEXT_ID(1, 2),
        .differentRouteMatchCallTextId = TEXT_ID(2, 2),
    },
    {
        .trainerId = TRAINER_AMY_AND_LIV_1,
        .unused = 2,
        .battleTopicTextIds = { TEXT_ID(1, 2), TEXT_ID(2, 2), TEXT_ID(3, 2) },
        .battleFrontierStreakTextId = TEXT_ID(1, 48),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 2),
        .differentRouteMatchCallTextId = TEXT_ID(2, 2),
    },
    {
        .trainerId = TRAINER_ERNEST_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 49),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_CORY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 63),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_EDWIN_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 7), TEXT_ID(2, 7), TEXT_ID(3, 7) },
        .battleFrontierStreakTextId = TEXT_ID(1, 50),
        .battleFrontierRecordStreakTextIndex = 7,
        .sameRouteMatchCallTextId = TEXT_ID(1, 7),
        .differentRouteMatchCallTextId = TEXT_ID(2, 7),
    },
    {
        .trainerId = TRAINER_LYDIA_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 52),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_ISAAC_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 51),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_GABRIELLE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 8), TEXT_ID(2, 8), TEXT_ID(3, 8) },
        .battleFrontierStreakTextId = TEXT_ID(1, 2),
        .battleFrontierRecordStreakTextIndex = 8,
        .sameRouteMatchCallTextId = TEXT_ID(1, 8),
        .differentRouteMatchCallTextId = TEXT_ID(2, 8),
    },
    {
        .trainerId = TRAINER_CATHERINE_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 54),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
    {
        .trainerId = TRAINER_JACKSON_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 5), TEXT_ID(2, 5), TEXT_ID(3, 5) },
        .battleFrontierStreakTextId = TEXT_ID(1, 53),
        .battleFrontierRecordStreakTextIndex = 5,
        .sameRouteMatchCallTextId = TEXT_ID(1, 5),
        .differentRouteMatchCallTextId = TEXT_ID(2, 5),
    },
    {
        .trainerId = TRAINER_HALEY_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 2), TEXT_ID(2, 2), TEXT_ID(3, 2) },
        .battleFrontierStreakTextId = TEXT_ID(1, 55),
        .battleFrontierRecordStreakTextIndex = 2,
        .sameRouteMatchCallTextId = TEXT_ID(1, 2),
        .differentRouteMatchCallTextId = TEXT_ID(2, 2),
    },
    {
        .trainerId = TRAINER_JAMES_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 1), TEXT_ID(2, 1), TEXT_ID(3, 1) },
        .battleFrontierStreakTextId = TEXT_ID(1, 56),
        .battleFrontierRecordStreakTextIndex = 1,
        .sameRouteMatchCallTextId = TEXT_ID(1, 1),
        .differentRouteMatchCallTextId = TEXT_ID(2, 1),
    },
    {
        .trainerId = TRAINER_TRENT_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 3), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 57),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_SAWYER_1,
        .unused = 0,
        .battleTopicTextIds = { TEXT_ID(1, 15), TEXT_ID(2, 3), TEXT_ID(3, 3) },
        .battleFrontierStreakTextId = TEXT_ID(1, 1),
        .battleFrontierRecordStreakTextIndex = 3,
        .sameRouteMatchCallTextId = TEXT_ID(1, 3),
        .differentRouteMatchCallTextId = TEXT_ID(2, 3),
    },
    {
        .trainerId = TRAINER_KIRA_AND_DAN_1,
        .unused = 1,
        .battleTopicTextIds = { TEXT_ID(1, 9), TEXT_ID(2, 9), TEXT_ID(3, 9) },
        .battleFrontierStreakTextId = TEXT_ID(1, 58),
        .battleFrontierRecordStreakTextIndex = 9,
        .sameRouteMatchCallTextId = TEXT_ID(1, 9),
        .differentRouteMatchCallTextId = TEXT_ID(2, 9),
    },
};

static const struct MatchCallText sMatchCallWildBattleTexts[] =
{
    { .text = BattleFrontier_PokeNav_2A971C, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9798, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9813, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A98A8, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9977, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A99FD, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9A78, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9AE8, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9BA7, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9C36, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9CC8, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9D44, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9DD7, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9E70, .stringVarFuncIds = { 0, 2, -1 } },
    { .text = BattleFrontier_PokeNav_2A9EFD, .stringVarFuncIds = { 0, 2, -1 } },
};

 static const struct MatchCallText sMatchCallNegativeBattleTexts[] =
 {
    { .text = BattleFrontier_PokeNav_2A9FAB, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA028, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA099, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA100, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA188, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA214, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA2A1, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA31B, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA3A8, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA442, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA4C5, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA520, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA5AD, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AA64D, .stringVarFuncIds = { 0, -1, -1 } },
};

static const struct MatchCallText sMatchCallPositiveBattleTexts[] =
{
    { .text = BattleFrontier_PokeNav_2AA6AF, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA730, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA77A, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA81C, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA88C, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA934, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AA9D3, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAA40, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAAE4, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAB8C, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAC25, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAC9D, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAD41, .stringVarFuncIds = { 0, 3, -1 } },
    { .text = BattleFrontier_PokeNav_2AAE00, .stringVarFuncIds = { 0, 3, -1 } },
};

static const struct MatchCallText sMatchCallSameRouteBattleRequestTexts[] =
{
    { .text = BattleFrontier_PokeNav_2AAE7F, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AAEF1, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AAF69, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB010, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB076, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB11A, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB1B4, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB23D, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB2E9, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB382, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB410, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB4B0, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB538, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB5E4, .stringVarFuncIds = { 0, 1, -1 } },
};

static const struct MatchCallText sMatchCallDifferentRouteBattleRequestTexts[] =
{
    { .text = BattleFrontier_PokeNav_2AB670, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB6CD, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB73C, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB808, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB8B7, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2AB95D, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABA03, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABA9F, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABB62, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABC26, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABCE9, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABDA2, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABE5E, .stringVarFuncIds = { 0, 1, -1 } },
    { .text = BattleFrontier_PokeNav_2ABF36, .stringVarFuncIds = { 0, 1, -1 } },
};

static const struct MatchCallText sMatchCallPersonalizedTexts[] =
{
    { .text = BattleFrontier_PokeNav_2AC009, .stringVarFuncIds = { 0,  1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC0BD, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC18C, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC228, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC30C, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC3B6, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC446, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC5C7, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC682, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC755, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC82C, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AC914, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ACA59, .stringVarFuncIds = { 0,  2, -1 } },
    { .text = BattleFrontier_PokeNav_2ACB02, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ACC3F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ACD2F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ACE1E, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ACF32, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2ACFBE, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD0AC, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD194, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD1DF, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD2A8, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD34F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD44E, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD53A, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD642, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AD801, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2AD92E, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2ADA8F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ADB9B, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ADC92, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ADE08, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2ADF07, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE0D9, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE1FD, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE327, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE3DA, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE489, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE5CD, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE698, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE704, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2AE78F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE859, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2AE8E6, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AE998, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AEA8F, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AEB77, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AEBFA, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AECC1, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AED52, .stringVarFuncIds = { 0,  1, -1 } },
    { .text = BattleFrontier_PokeNav_2AEE35, .stringVarFuncIds = { 0,  3, -1 } },
    { .text = BattleFrontier_PokeNav_2AEEF4, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AEFDA, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF0E7, .stringVarFuncIds = { 0,  1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF1B8, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF2C4, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF371, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF480, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF671, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF7D8, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AF8F7, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AFA39, .stringVarFuncIds = { 0, -1, -1 } },
    { .text = BattleFrontier_PokeNav_2AFB26, .stringVarFuncIds = { 0, -1, -1 } },
};

static const struct MatchCallText sMatchCallBattleFrontierStreakTexts[] =
{
    { .text = BattleFrontier_PokeNav_2AFC07, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFC78, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFCFF, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFDA7, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFE3D, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFECA, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFF64, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2AFFF0, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B00B5, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0129, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B01A5, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0232, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B02D9, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0366, .stringVarFuncIds = { 0, 4, 5 } },
};

static const struct MatchCallText sMatchCallBattleFrontierRecordStreakTexts[] =
{
    { .text = BattleFrontier_PokeNav_2B03E6, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0457, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B04DE, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0586, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B061C, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B06A3, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B073B, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B07C7, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0894, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0908, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0984, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0A11, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0AB8, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0B45, .stringVarFuncIds = { 0, 4, 5 } },
};

static const struct MatchCallText sMatchCallBattleDomeTexts[] =
{
    { .text = BattleFrontier_PokeNav_2B0BC5, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0C4A, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0CC7, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0D4A, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0DC8, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0E35, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0ED1, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B0F72, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B102A, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B10A7, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1121, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B11D3, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B124D, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B12D0, .stringVarFuncIds = { 0, 4, 5 } },
};

static const struct MatchCallText sMatchCallBattlePikeTexts[] =
{
    { .text = BattleFrontier_PokeNav_2B1347, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B13B1, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B142B, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B14B4, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1525, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B158E, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1622, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B169D, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1775, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B17F8, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1877, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1946, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B19C7, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1A4C, .stringVarFuncIds = { 0, 4, 5 } },
};

static const struct MatchCallText sMatchCallBattlePyramidTexts[] =
{
    { .text = BattleFrontier_PokeNav_2B1ACA, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1B50, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1BE4, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1C6A, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1CCF, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1D38, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1DCD, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1E4B, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1F24, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B1FA8, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B2022, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B20F3, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B2175, .stringVarFuncIds = { 0, 4, 5 } },
    { .text = BattleFrontier_PokeNav_2B21FC, .stringVarFuncIds = { 0, 4, 5 } },
};

static const struct MatchCallText *const sMatchCallBattleTopics[] =
{
    sMatchCallWildBattleTexts,
    sMatchCallNegativeBattleTexts,
    sMatchCallPositiveBattleTexts,
};

static const struct MatchCallText *const sMatchCallBattleRequestTopics[] =
{
    sMatchCallSameRouteBattleRequestTexts,
    sMatchCallDifferentRouteBattleRequestTexts,
};

static const struct MatchCallText *const sMatchCallGeneralTopics[] =
{
    sMatchCallPersonalizedTexts,
    sMatchCallBattleFrontierStreakTexts,
    sMatchCallBattleFrontierRecordStreakTexts,
    sMatchCallBattleDomeTexts,
    sMatchCallBattlePikeTexts,
    sMatchCallBattlePyramidTexts,
};

extern const u8 gUnknown_082A5C9C[];
extern const u8 gUnknown_082A5D2C[];
extern const u8 gUnknown_082A633D[];

void InitMatchCallCounters(void)
{
    RtcCalcLocalTime();
    gMatchCallState.minutes = GetCurrentTotalMinutes(&gLocalTime) + 10;
    gMatchCallState.stepCounter = 0;
}

static u32 GetCurrentTotalMinutes(struct Time *time)
{
    return time->days * 1440 + time->hours * 60 + time->minutes;
}

static bool32 UpdateMatchCallMinutesCounter(void)
{
    int curMinutes;
    RtcCalcLocalTime();
    curMinutes = GetCurrentTotalMinutes(&gLocalTime);
    if (gMatchCallState.minutes > curMinutes || curMinutes - gMatchCallState.minutes > 9)
    {
        gMatchCallState.minutes = curMinutes;
        return TRUE;
    }

    return FALSE;
}

static bool32 CheckMatchCallChance(void)
{
    int callChance = 1;
    if (!GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_EGG) && GetMonAbility(&gPlayerParty[0]) == ABILITY_LIGHTNING_ROD)
        callChance = 2;
    
    if (Random() % 10 < callChance * 3) 
        return TRUE;
    else
        return FALSE;
}

static bool32 MapAllowsMatchCall(void)
{
    if (!Overworld_MapTypeAllowsTeleportAndFly(gMapHeader.mapType) || gMapHeader.regionMapSectionId == MAPSEC_SAFARI_ZONE)
        return FALSE;
    
    if (gMapHeader.regionMapSectionId == MAPSEC_SOOTOPOLIS_CITY
     && FlagGet(FLAG_HIDE_SOOTOPOLIS_CITY_RAYQUAZA) == TRUE
     && FlagGet(FLAG_0x0DC) == FALSE)
        return FALSE;

    if (gMapHeader.regionMapSectionId == MAPSEC_MT_CHIMNEY
     && FlagGet(FLAG_0x0CF) == TRUE
     && FlagGet(FLAG_0x08B) == FALSE)
        return FALSE;

    return TRUE;
}

static bool32 UpdateMatchCallStepCounter(void)
{
    if (++gMatchCallState.stepCounter >= 10)
    {
        gMatchCallState.stepCounter = 0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool32 SelectMatchCallTrainer(void)
{
    u32 matchCallId;
    u32 numRegistered = GetNumRegisteredNPCs();
    if (!numRegistered)
        return FALSE;

    gMatchCallState.trainerId = GetActiveMatchCallTrainerId(Random() % numRegistered);
    gMatchCallState.triggeredFromScript = 0;
    if (gMatchCallState.trainerId == REMATCH_TABLE_ENTRIES)
        return FALSE;

    matchCallId = GetTrainerMatchCallId(gMatchCallState.trainerId);
    if (GetRematchTrainerLocation(matchCallId) == gMapHeader.regionMapSectionId && !TrainerIsEligibleForRematch(matchCallId))
        return FALSE;

    return TRUE;
}

static u32 GetNumRegisteredNPCs(void)
{
    u32 i, count;
    for (i = 0, count = 0; i < 64; i++)
    {
        if (FlagGet(FLAG_MATCH_CALL_REGISTERED + i))
            count++;
    }

    return count;
}

static u32 GetActiveMatchCallTrainerId(u32 activeMatchCallId)
{
    u32 i;
    for (i = 0; i < 64; i++)
    {
        if (FlagGet(FLAG_MATCH_CALL_REGISTERED + i))
        {
            if (!activeMatchCallId)
                return gRematchTable[i].trainerIds[0];

            activeMatchCallId--;
        }
    }

    return REMATCH_TABLE_ENTRIES;
}

bool32 TryStartMatchCall(void)
{
    if (FlagGet(FLAG_HAS_MATCH_CALL) && UpdateMatchCallStepCounter() && UpdateMatchCallMinutesCounter()
     && CheckMatchCallChance() && MapAllowsMatchCall() && SelectMatchCallTrainer())
    {
        StartMatchCall();
        return TRUE;
    }

    return FALSE;
}

void StartMatchCallFromScript(void)
{
    gMatchCallState.triggeredFromScript = 1;
    StartMatchCall();
}

bool8 IsMatchCallTaskActive(void)
{
    return FuncIsActiveTask(ExecuteMatchCall);
}

static void StartMatchCall(void)
{
    if (!gMatchCallState.triggeredFromScript)
    {
        ScriptContext2_Enable();
        FreezeEventObjects();
        sub_808B864();
        sub_808BCF4();
    }

    PlaySE(SE_TOREEYE);
    CreateTask(ExecuteMatchCall, 1);
}

static const u16 sUnknown_0860EA4C[] = INCBIN_U16("graphics/unknown/unknown_60EA4C.gbapal");
static const u8 sUnknown_0860EA6C[] = INCBIN_U8("graphics/interface/menu_border.4bpp");
static const u16 sPokeNavIconPalette[] = INCBIN_U16("graphics/pokenav/icon.gbapal");
static const u32 sPokeNavIconGfx[] = INCBIN_U32("graphics/pokenav/icon.4bpp.lz");

static const u8 sText_PokenavCallEllipsis[] = _("………………\p");

static bool32 (*const sMatchCallTaskFuncs[])(u8) =
{
    LoadMatchCallWindowGfx,
    MoveMatchCallWindowToVram,
    PrintMatchCallIntroEllipsis,
    sub_81962B0,
    sub_81962D8,
    sub_8196330,
    sub_8196390,
    sub_81963F0,
};

static void ExecuteMatchCall(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (sMatchCallTaskFuncs[taskData[0]](taskId))
    {
        taskData[0]++;
        taskData[1] = 0;
        if ((u16)taskData[0] > 7)
            DestroyTask(taskId);
    }
}

static const struct WindowTemplate sMatchCallTextWindow =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 28,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x200
};

static bool32 LoadMatchCallWindowGfx(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    taskData[2] = AddWindow(&sMatchCallTextWindow);
    if (taskData[2] == 0xFF)
    {
        DestroyTask(taskId);
        return FALSE;
    }

    if (LoadBgTiles(0, sUnknown_0860EA6C, sizeof(sUnknown_0860EA6C), 0x270) == 0xFFFF)
    {
        RemoveWindow(taskData[2]);
        DestroyTask(taskId);
        return FALSE;
    }

    if (!decompress_and_copy_tile_data_to_vram(0, sPokeNavIconGfx, 0, 0x279, 0))
    {
        RemoveWindow(taskData[2]);
        DestroyTask(taskId);
        return FALSE;
    }

    FillWindowPixelBuffer(taskData[2], 0x88);
    LoadPalette(sUnknown_0860EA4C, 0xE0, 0x20);
    LoadPalette(sPokeNavIconPalette, 0xF0, 0x20);
    ChangeBgY(0, -0x2000, 0);
    return TRUE;
}

static bool32 MoveMatchCallWindowToVram(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (free_temp_tile_data_buffers_if_possible())
        return FALSE;

    PutWindowTilemap(taskData[2]);
    DrawMatchCallTextBoxBorder(taskData[2], 0x270, 14);
    WriteSequenceToBgTilemapBuffer(0, 0xF279, 1, 15, 4, 4, 17, 1);
    taskData[5] = CreateTask(sub_8196694, 10);
    CopyWindowToVram(taskData[2], 2);
    CopyBgTilemapBufferToVram(0);
    return TRUE;
}

static bool32 PrintMatchCallIntroEllipsis(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (!IsDma3ManagerBusyWithBgCopy())
    {
        InitMatchCallTextPrinter(taskData[2], sText_PokenavCallEllipsis);
        return TRUE;
    }

    return FALSE;
}

static bool32 sub_81962B0(u8 taskId)
{
    if (ChangeBgY(0, 0x600, 1) >= 0)
    {
        ChangeBgY(0, 0, 0);
        return TRUE;
    }

    return FALSE;
}

static bool32 sub_81962D8(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (!ExecuteMatchCallTextPrinter(taskData[2]))
    {
        FillWindowPixelBuffer(taskData[2], 0x88);
        if (!gMatchCallState.triggeredFromScript)
            SelectMatchCallMessage(gMatchCallState.trainerId, gStringVar4);

        InitMatchCallTextPrinter(taskData[2], gStringVar4);
        return TRUE;
    }

    return FALSE;
}

static bool32 sub_8196330(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (!ExecuteMatchCallTextPrinter(taskData[2]) && !IsSEPlaying() && gMain.newKeys & (A_BUTTON | B_BUTTON))
    {
        FillWindowPixelBuffer(taskData[2], 0x88);
        CopyWindowToVram(taskData[2], 2);
        PlaySE(SE_TOREOFF);
        return TRUE;
    }

    return FALSE;
}

static bool32 sub_8196390(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (ChangeBgY(0, 0x600, 2) <= -0x2000)
    {
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 14, 30, 6);
        DestroyTask(taskData[5]);
        RemoveWindow(taskData[2]);
        CopyBgTilemapBufferToVram(0);
        return TRUE;
    }

    return FALSE;
}

static bool32 sub_81963F0(u8 taskId)
{
    u8 playerObjectId;
    if (!IsDma3ManagerBusyWithBgCopy() && !IsSEPlaying())
    {
        ChangeBgY(0, 0, 0);
        if (!gMatchCallState.triggeredFromScript)
        {
            sub_81973A4();
            playerObjectId = GetEventObjectIdByLocalIdAndMap(EVENT_OBJ_ID_PLAYER, 0, 0);
            EventObjectClearHeldMovementIfFinished(&gEventObjects[playerObjectId]);
            sub_80D338C();
            UnfreezeEventObjects();
            ScriptContext2_Disable();
        }

        return TRUE;
    }

    return FALSE;
}

static void DrawMatchCallTextBoxBorder(u32 windowId, u32 tileOffset, u32 paletteId)
{
    int bg, x, y, width, height;
    int tileNum;

    bg = GetWindowAttribute(windowId, WINDOW_BG);
    x = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    y = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    height = GetWindowAttribute(windowId, WINDOW_HEIGHT);
    tileNum = tileOffset + GetBgAttribute(bg, BG_ATTR_BASETILE);

    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 0), x - 1, y - 1, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 1), x, y - 1, width, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 2), x + width, y - 1, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 3), x - 1, y, 1, height);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 4), x + width, y, 1, height);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 5), x - 1, y + height, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 6), x, y + height, width, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 7), x + width, y + height, 1, 1);
}

static void InitMatchCallTextPrinter(int windowId, const u8 *str)
{
    struct TextPrinterTemplate printerTemplate;
    printerTemplate.currentChar = str;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = 1;
    printerTemplate.x = 32;
    printerTemplate.y = 1;
    printerTemplate.currentX = 32;
    printerTemplate.currentY = 1;
    printerTemplate.letterSpacing = 0;
    printerTemplate.lineSpacing = 0;
    printerTemplate.unk = 0;
    printerTemplate.fgColor = 10;
    printerTemplate.bgColor = 8;
    printerTemplate.shadowColor = 14;
    gTextFlags.useAlternateDownArrow = 0;

    AddTextPrinter(&printerTemplate, GetPlayerTextSpeedDelay(), NULL);
}

static bool32 ExecuteMatchCallTextPrinter(int windowId)
{
    if (gMain.heldKeys & A_BUTTON)
        gTextFlags.canABSpeedUpPrint = 1;
    else
        gTextFlags.canABSpeedUpPrint = 0;

    RunTextPrinters();
    return IsTextPrinterActive(windowId);
}

static void sub_8196694(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;
    if (++taskData[0] > 8)
    {
        taskData[0] = 0;
        if (++taskData[1] > 7)
            taskData[1] = 0;

        taskData[2] = (taskData[1] * 16) + 0x279;
        WriteSequenceToBgTilemapBuffer(0, taskData[2] | ~0xFFF, 1, 15, 4, 4, 17, 1);
        CopyBgTilemapBufferToVram(0);
    }
}

static bool32 TrainerIsEligibleForRematch(int matchCallId)
{
    return gSaveBlock1Ptr->trainerRematches[matchCallId] > 0;
}

static u16 GetRematchTrainerLocation(int matchCallId)
{
    const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(gRematchTable[matchCallId].mapGroup, gRematchTable[matchCallId].mapNum);
    return mapHeader->regionMapSectionId;
}

static u32 GetNumRematchTrainersFought(void)
{
    u32 i, count;
    for (i = 0, count = 0; i < 64; i++)
    {
        if (HasTrainerBeenFought(gRematchTable[i].trainerIds[0]))
            count++;
    }

    return count;
}

static u32 sub_8196774(int arg0)
{
    u32 i, count;

    for (i = 0, count = 0; i < REMATCH_TABLE_ENTRIES; i++)
    {
        if (HasTrainerBeenFought(gRematchTable[i].trainerIds[0]))
        {
            if (count == arg0)
                return i;

            count++;
        }
    }

    return REMATCH_TABLE_ENTRIES;
}

bool32 SelectMatchCallMessage(int trainerId, u8 *str)
{
    u32 matchCallId;
    const struct MatchCallText *matchCallText;
    bool32 retVal = FALSE;

    matchCallId = GetTrainerMatchCallId(trainerId);
    gBattleFrontierStreakInfo.facilityId = 0;
    if (TrainerIsEligibleForRematch(matchCallId)
     && GetRematchTrainerLocation(matchCallId) == gMapHeader.regionMapSectionId)
    {
        matchCallText = GetSameRouteMatchCallText(matchCallId, str);
    }
    else if (sub_8196D74(matchCallId))
    {
        matchCallText = GetDifferentRouteMatchCallText(matchCallId, str);
        retVal = TRUE;
        UpdateRematchIfDefeated(matchCallId);
    }
    else if (Random() % 3)
    {
        matchCallText = GetBattleMatchCallText(matchCallId, str);
    }
    else
    {
        matchCallText = GetGeneralMatchCallText(matchCallId, str);
    }

    BuildMatchCallString(matchCallId, matchCallText, str);
    return retVal;
}

static int GetTrainerMatchCallId(int trainerId)
{
    int i = 0;
    while (1)
    {
        if (sMatchCallTrainers[i].trainerId == trainerId)
            return i;
        else
            i++;
    }
}

static const struct MatchCallText *GetSameRouteMatchCallText(int matchCallId, u8 *str)
{
    u16 textId = sMatchCallTrainers[matchCallId].sameRouteMatchCallTextId;
    int mask = 0xFF;
    u32 topic = (textId >> 8) - 1;
    u32 id = (textId & mask) - 1;
    return &sMatchCallBattleRequestTopics[topic][id];
}

static const struct MatchCallText *GetDifferentRouteMatchCallText(int matchCallId, u8 *str)
{
    u16 textId = sMatchCallTrainers[matchCallId].differentRouteMatchCallTextId;
    int mask = 0xFF;
    u32 topic = (textId >> 8) - 1;
    u32 id = (textId & mask) - 1;
    return &sMatchCallBattleRequestTopics[topic][id];
}

static const struct MatchCallText *GetBattleMatchCallText(int matchCallId, u8 *str)
{
    int mask;
    u32 textId, topic, id;
    
    topic = Random() % 3;
    textId = sMatchCallTrainers[matchCallId].battleTopicTextIds[topic];
    if (!textId)
        SpriteCallbackDummy(NULL); // leftover debugging ???

    mask = 0xFF;
    topic = (textId >> 8) - 1;
    id = (textId & mask) - 1;
    return &sMatchCallBattleTopics[topic][id];
}

static const struct MatchCallText *GetGeneralMatchCallText(int matchCallId, u8 *str)
{
    int i;
    int count;
    u32 topic, id;
    u16 rand;
    u16 var3;

    rand = Random();
    if (!(rand & 1))
    {
        for (count = 0, i = 0; i < NUM_FRONTIER_FACILITIES; i++)
        {
            if (GetFrontierStreakInfo(i, &topic) > 1)
                count++;
        }

        if (count)
        {
            count = Random() % count;
            for (i = 0; i < NUM_FRONTIER_FACILITIES; i++)
            {
                gBattleFrontierStreakInfo.streak = GetFrontierStreakInfo(i, &topic);
                if (gBattleFrontierStreakInfo.streak < 2)
                    continue;

                if (!count)
                    break;

                count--;
            }

            gBattleFrontierStreakInfo.facilityId = i;
            id = sMatchCallTrainers[matchCallId].battleFrontierRecordStreakTextIndex - 1;
            return &sMatchCallGeneralTopics[topic][id];
        }
    }

    topic = (sMatchCallTrainers[matchCallId].battleFrontierStreakTextId >> 8) - 1;
    id = (sMatchCallTrainers[matchCallId].battleFrontierStreakTextId & 0xFF) - 1;
    return &sMatchCallGeneralTopics[topic][id];
}

static void BuildMatchCallString(int matchCallId, const struct MatchCallText *matchCallText, u8 *str)
{
    PopulateMatchCallStringVars(matchCallId, matchCallText->stringVarFuncIds);
    StringExpandPlaceholders(str, matchCallText->text);
}

static u8 *const sMatchCallTextStringVars[] = { gStringVar1, gStringVar2, gStringVar3 };

static void PopulateMatchCallStringVars(int matchCallId, const s8 *stringVarFuncIds)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        if (stringVarFuncIds[i] >= 0)
            PopulateMatchCallStringVar(matchCallId, stringVarFuncIds[i], sMatchCallTextStringVars[i]);
    }
}

static void (*const sPopulateMatchCallStringVarFuncs[])(int, u8 *) =
{
    PopulateTrainerName,
    PopulateMapName,
    PopulateSpeciesFromTrainerLocation,
    PopulateSpeciesFromTrainerParty,
    PopulateBattleFrontierFacilityName,
    PopulateBattleFrontierStreak,
};

static void PopulateMatchCallStringVar(int matchCallId, int funcId, u8 *destStr)
{
    sPopulateMatchCallStringVarFuncs[funcId](matchCallId, destStr);
}

static const struct MultiTrainerMatchCallText sMultiTrainerMatchCallTexts[] =
{
    { .trainerId = TRAINER_KIRA_AND_DAN_1, .text = gText_Kira },
    { .trainerId = TRAINER_AMY_AND_LIV_1,  .text = gText_Amy },
    { .trainerId = TRAINER_JOHN_AND_JAY_1, .text = gText_John },
    { .trainerId = TRAINER_LILA_AND_ROY_1, .text = gText_Roy },
    { .trainerId = TRAINER_GABBY_AND_TY_1, .text = gText_Gabby },
    { .trainerId = TRAINER_ANNA_AND_MEG_1, .text = gText_Anna },
};

static void PopulateTrainerName(int matchCallId, u8 *destStr)
{
    u32 i;
    u16 trainerId = sMatchCallTrainers[matchCallId].trainerId;
    for (i = 0; i < 6; i++)
    {
        if (sMultiTrainerMatchCallTexts[i].trainerId == trainerId)
        {
            StringCopy(destStr, sMultiTrainerMatchCallTexts[i].text);
            return;
        }
    }

    StringCopy(destStr, gTrainers[trainerId].trainerName);
}

static void PopulateMapName(int matchCallId, u8 *destStr)
{
    GetMapName(destStr, GetRematchTrainerLocation(matchCallId), 0);
}

static u8 GetLandEncounterSlot(void)
{
    int rand = Random() % 100;
    if (rand < 20)
        return 0;
    else if (rand >= 20 && rand < 40)
        return 1;
    else if (rand >= 40 && rand < 50)
        return 2;
    else if (rand >= 50 && rand < 60)
        return 3;
    else if (rand >= 60 && rand < 70)
        return 4;
    else if (rand >= 70 && rand < 80)
        return 5;
    else if (rand >= 80 && rand < 85)
        return 6;
    else if (rand >= 85 && rand < 90)
        return 7;
    else if (rand >= 90 && rand < 94)
        return 8;
    else if (rand >= 94 && rand < 98)
        return 9;
    else if (rand >= 98 && rand < 99)
        return 10;
    else
        return 11;
}

static u8 GetWaterEncounterSlot(void)
{
    int rand = Random() % 100;
    if (rand < 60)
        return 0;
    else if (rand >= 60 && rand < 90)
        return 1;
    else if (rand >= 90 && rand < 95)
        return 2;
    else if (rand >= 95 && rand < 99)
        return 3;
    else
        return 4;
}

static void PopulateSpeciesFromTrainerLocation(int matchCallId, u8 *destStr)
{
    u16 species[2];
    int numSpecies;
    u8 slot;
    int i = 0;

    if (gWildMonHeaders[i].mapGroup != MAP_GROUP(UNDEFINED)) // ??? This check is nonsense.
    {
        while (gWildMonHeaders[i].mapGroup != MAP_GROUP(UNDEFINED))
        {
            if (gWildMonHeaders[i].mapGroup == gRematchTable[matchCallId].mapGroup
             && gWildMonHeaders[i].mapNum == gRematchTable[matchCallId].mapNum)
                break;

            i++;
        }

        if (gWildMonHeaders[i].mapGroup != MAP_GROUP(UNDEFINED))
        {
            numSpecies = 0;
            if (gWildMonHeaders[i].landMonsInfo)
            {
                slot = GetLandEncounterSlot();
                species[numSpecies] = gWildMonHeaders[i].landMonsInfo->wildPokemon[slot].species;
                numSpecies++;
            }

            if (gWildMonHeaders[i].waterMonsInfo)
            {
                slot = GetWaterEncounterSlot();
                species[numSpecies] = gWildMonHeaders[i].waterMonsInfo->wildPokemon[slot].species;
                numSpecies++;
            }

            if (numSpecies)
            {
                StringCopy(destStr, gSpeciesNames[species[Random() % numSpecies]]);
                return;
            }
        }
    }

    destStr[0] = EOS;
}

static void PopulateSpeciesFromTrainerParty(int matchCallId, u8 *destStr)
{
    u16 trainerId;
    union TrainerMonPtr party;
    u8 monId;
    const u8 *speciesName;

    trainerId = GetLastBeatenRematchTrainerId(sMatchCallTrainers[matchCallId].trainerId);
    party = gTrainers[trainerId].party;
    monId = Random() % gTrainers[trainerId].partySize;

    switch (gTrainers[trainerId].partyFlags)
    {
    case 0:
    default:
        speciesName = gSpeciesNames[party.NoItemDefaultMoves[monId].species];
        break;
    case F_TRAINER_PARTY_CUSTOM_MOVESET:
        speciesName = gSpeciesNames[party.NoItemCustomMoves[monId].species];
        break;
    case F_TRAINER_PARTY_HELD_ITEM:
        speciesName = gSpeciesNames[party.ItemDefaultMoves[monId].species];
        break;
    case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
        speciesName = gSpeciesNames[party.ItemCustomMoves[monId].species];
        break;
    }

    StringCopy(destStr, speciesName);
}

static const u8 *const sBattleFrontierFacilityNames[] =
{
    gText_BattleTower2,
    gText_BattleDome,
    gText_BattlePalace,
    gText_BattleArena,
    gText_BattlePike,
    gText_BattleFactory,
    gText_BattlePyramid,
};

static void PopulateBattleFrontierFacilityName(int matchCallId, u8 *destStr)
{
    StringCopy(destStr, sBattleFrontierFacilityNames[gBattleFrontierStreakInfo.facilityId]);
}

static void PopulateBattleFrontierStreak(int matchCallId, u8 *destStr)
{
    int i = 0;
    int streak = gBattleFrontierStreakInfo.streak;
    while (streak != 0)
    {
        streak /= 10;
        i++;
    }
    
    ConvertIntToDecimalStringN(destStr, gBattleFrontierStreakInfo.streak, 0, i);
}

static const u16 sBadgeFlags[] =
{
    FLAG_BADGE01_GET,
    FLAG_BADGE02_GET,
    FLAG_BADGE03_GET,
    FLAG_BADGE04_GET,
    FLAG_BADGE05_GET,
    FLAG_BADGE06_GET,
    FLAG_BADGE07_GET,
    FLAG_BADGE08_GET,
};

static int GetNumOwnedBadges(void)
{
    u32 i;

    for (i = 0; i < 8; i++)
    {
        if (!FlagGet(sBadgeFlags[i]))
            break;
    }

    return i;
}

static bool32 sub_8196D74(int matchCallId)
{
    int dayCount;
    int otId;
    u16 easyChatWord;
    int numRematchTrainersFought;
    int var0, var1, var2;

    if (GetNumOwnedBadges() < 5)
        return FALSE;

    dayCount = RtcGetLocalDayCount();
    otId = GetTrainerId(gSaveBlock2Ptr->playerTrainerId) & 0xFFFF;

    easyChatWord = gSaveBlock1Ptr->easyChatPairs[0].unk2;
    numRematchTrainersFought = GetNumRematchTrainersFought();
    var0 = (numRematchTrainersFought * 13) / 10;
    var1 = ((dayCount ^ easyChatWord) + (easyChatWord ^ GetGameStat(GAME_STAT_TRAINER_BATTLES))) ^ otId;
    var2 = var1 % var0;
    if (var2 < numRematchTrainersFought)
    {
        if (sub_8196774(var2) == matchCallId)
            return TRUE;
    }

    return FALSE;
}

static u16 GetFrontierStreakInfo(u16 facilityId, u32 *topicTextId)
{
    int i;
    int j;
    u16 streak = 0;

    switch (facilityId)
    {
    case FRONTIER_FACILITY_DOME:
        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 2; j++)
            {
                if (streak < gSaveBlock2Ptr->frontier.domeRecordWinStreaks[i][j])
                    streak = gSaveBlock2Ptr->frontier.domeRecordWinStreaks[i][j];
            }
        }
        *topicTextId = 3;
        break;
    case FRONTIER_FACILITY_FACTORY:
        for (i = 0; i < 2; i++)
        {
            // BUG: should be looking at battle factory records.
            if (streak < gSaveBlock2Ptr->frontier.pikeRecordStreaks[i])
                streak = gSaveBlock2Ptr->frontier.pikeRecordStreaks[i];
        }
        *topicTextId = 4;
        break;
    case FRONTIER_FACILITY_TOWER:
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 2; j++)
            {
                if (streak < gSaveBlock2Ptr->frontier.towerRecordWinStreaks[i][j])
                    streak = gSaveBlock2Ptr->frontier.towerRecordWinStreaks[i][j];
            }
        }
        *topicTextId = 2;
        break;
    case FRONTIER_FACILITY_PALACE:
        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 2; j++)
            {
                if (streak < gSaveBlock2Ptr->frontier.palaceRecordWinStreaks[i][j])
                    streak = gSaveBlock2Ptr->frontier.palaceRecordWinStreaks[i][j];
            }
        }
        *topicTextId = 2;
        break;
    case FRONTIER_FACILITY_PIKE:
        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 2; j++)
            {
                // BUG: should be looking at battle pike records.
                if (streak < gSaveBlock2Ptr->frontier.factoryRecordWinStreaks[i][j])
                    streak = gSaveBlock2Ptr->frontier.factoryRecordWinStreaks[i][j];
            }
        }
        *topicTextId = 2;
        break;
    case FRONTIER_FACILITY_ARENA:
        for (i = 0; i < 2; i++)
        {
            if (streak < gSaveBlock2Ptr->frontier.arenaRecordStreaks[i])
                streak = gSaveBlock2Ptr->frontier.arenaRecordStreaks[i];
        }
        *topicTextId = 2;
        break;
    case FRONTIER_FACILITY_PYRAMID:
        for (i = 0; i < 2; i++)
        {
            if (streak < gSaveBlock2Ptr->frontier.pyramidRecordStreaks[i])
                streak = gSaveBlock2Ptr->frontier.pyramidRecordStreaks[i];
        }
        *topicTextId = 5;
        break;
    }

    return streak;
}

static u8 GetPokedexRatingLevel(u16 numSeen)
{
    if (numSeen < 10)
        return 0;
    if (numSeen < 20)
        return 1;
    if (numSeen < 30)
        return 2;
    if (numSeen < 40)
        return 3;
    if (numSeen < 50)
        return 4;
    if (numSeen < 60)
        return 5;
    if (numSeen < 70)
        return 6;
    if (numSeen < 80)
        return 7;
    if (numSeen < 90)
        return 8;
    if (numSeen < 100)
        return 9;
    if (numSeen < 110)
        return 10;
    if (numSeen < 120)
        return 11;
    if (numSeen < 130)
        return 12;
    if (numSeen < 140)
        return 13;
    if (numSeen < 150)
        return 14;
    if (numSeen < 160)
        return 15;
    if (numSeen < 170)
        return 16;
    if (numSeen < 180)
        return 17;
    if (numSeen < 190)
        return 18;
    if (numSeen < 200)
        return 19;
    
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_DEOXYS), FLAG_GET_CAUGHT))
        numSeen--;
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_JIRACHI), FLAG_GET_CAUGHT))
        numSeen--;

    if (numSeen < 200)
        return 19;
    else
        return 20;
}

static const u8 *const sBirchDexRatingTexts[] =
{
    gBirchDexRatingText_LessThan10,
    gBirchDexRatingText_LessThan20,
    gBirchDexRatingText_LessThan30,
    gBirchDexRatingText_LessThan40,
    gBirchDexRatingText_LessThan50,
    gBirchDexRatingText_LessThan60,
    gBirchDexRatingText_LessThan70,
    gBirchDexRatingText_LessThan80,
    gBirchDexRatingText_LessThan90,
    gBirchDexRatingText_LessThan100,
    gBirchDexRatingText_LessThan110,
    gBirchDexRatingText_LessThan120,
    gBirchDexRatingText_LessThan130,
    gBirchDexRatingText_LessThan140,
    gBirchDexRatingText_LessThan150,
    gBirchDexRatingText_LessThan160,
    gBirchDexRatingText_LessThan170,
    gBirchDexRatingText_LessThan180,
    gBirchDexRatingText_LessThan190,
    gBirchDexRatingText_LessThan200,
    gBirchDexRatingText_DexCompleted,
};

void sub_8197080(u8 *destStr)
{
    int numSeen, numCaught;
    u8 *str;
    u8 dexRatingLevel;

    u8 *buffer = Alloc(0x3E8);
    if (!buffer)
    {
        destStr[0] = EOS;
        return;
    }

    numSeen = GetHoennPokedexCount(FLAG_GET_SEEN);
    numCaught = GetHoennPokedexCount(FLAG_GET_CAUGHT);
    ConvertIntToDecimalStringN(gStringVar1, numSeen, 0, 3);
    ConvertIntToDecimalStringN(gStringVar2, numCaught, 0, 3);
    dexRatingLevel = GetPokedexRatingLevel(numCaught);
    str = StringCopy(buffer, gUnknown_082A5C9C);
    str[0] = CHAR_PROMPT_CLEAR;
    str++;
    str = StringCopy(str, gUnknown_082A5D2C);
    str[0] = CHAR_PROMPT_CLEAR;
    str++;
    StringCopy(str, sBirchDexRatingTexts[dexRatingLevel]);
    str = StringExpandPlaceholders(destStr, buffer);

    if (IsNationalPokedexEnabled())
    {
        str[0] = CHAR_PROMPT_CLEAR;
        str++;
        numSeen = GetNationalPokedexCount(FLAG_GET_SEEN);
        numCaught = GetNationalPokedexCount(FLAG_GET_CAUGHT);
        ConvertIntToDecimalStringN(gStringVar1, numSeen, 0, 3);
        ConvertIntToDecimalStringN(gStringVar2, numCaught, 0, 3);
        StringExpandPlaceholders(str, gUnknown_082A633D);
    }

    Free(buffer);
}

void sub_8197184(u8 windowId, u32 destOffset, u32 paletteId)
{
    u8 bg = GetWindowAttribute(windowId, WINDOW_BG);
    LoadBgTiles(bg, sUnknown_0860EA6C, 0x100, destOffset);
    LoadPalette(sUnknown_0860EA4C, paletteId << 4, 0x20);
}

void sub_81971C4(u32 windowId, u32 tileOffset, u32 paletteId)
{
    DrawMatchCallTextBoxBorder(windowId, tileOffset, paletteId);
}
