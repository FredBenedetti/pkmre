
// Includes
#include "global.h"
#include "rng.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_camera.h"
#include "strings.h"
#include "string_util.h"
#include "international_string_util.h"
#include "field_message_box.h"
#include "easy_chat.h"
#include "species.h"
#include "moves.h"
#include "battle.h"
#include "items.h"
#include "main.h"
#include "tv.h"

// Static type declarations

// Static RAM declarations

extern EWRAM_DATA u8 sTVShowState;
extern s8 gUnknown_030060BC;

// Static ROM declarations

extern const u8 *const gTVBravoTrainerTextGroup[];
extern const u8 *const gTVBravoTrainerBattleTowerTextGroup[];

void ClearPokemonNews(void);
u8 GetTVChannelByShowType(u8);
void CopyContestCategoryToStringVar(u8, u8);
void CopyContestRankToStringVar(u8, u8);
void TV_ConvertNumberToOrdinal(u8, u32);
static u8 FindFirstActiveTVShowThatIsNotAMassOutbreak(void);
u8 CheckForBigMovieOrEmergencyNewsOnTV(void);
static void SetTVMetatilesOnMap(int, int, u16);
u8 FindAnyTVNewsOnTheAir(void);
static bool8 IsTVShowInSearchOfTrainersAiring(void);
void TakeTVShowInSearchOfTrainersOffTheAir(void);

void sub_80EED88(void);
void sub_80ED718(void);
void sub_80EC8FC(void);
void sub_80EC8A4(void);
bool8 sub_80EFB38(u16);
s8 sub_80EFB08(TVShow *);
bool8 sub_80EF46C(u8, u8);
u32 sub_81DB604(const u8 *);
void sub_80EC9E8(TVShow *);

// .rodata

// .text

void ClearTVShowData(void)
{
    u8 i;
    u8 j;

    for (i = 0; i < ARRAY_COUNT(gSaveBlock1Ptr->tvShows); i ++)
    {
        gSaveBlock1Ptr->tvShows[i].common.kind = 0;
        gSaveBlock1Ptr->tvShows[i].common.active = 0;
        for (j = 0; j < sizeof(TVShow) - 2; j ++)
        {
            gSaveBlock1Ptr->tvShows[i].common.pad02[j] = 0;
        }
    }
    ClearPokemonNews();
}

u8 special_0x44(void)
{
    u8 i;
    u8 j;
    u8 selIdx;
    TVShow *show;

    for (i = 5; i < ARRAY_COUNT(gSaveBlock1Ptr->tvShows) - 1; i ++)
    {
        if (gSaveBlock1Ptr->tvShows[i].common.kind == 0)
        {
            break;
        }
    }
    j = Random() % i;
    selIdx = j;
    do
    {
        if (GetTVChannelByShowType(gSaveBlock1Ptr->tvShows[j].common.kind) != 4)
        {
            if (gSaveBlock1Ptr->tvShows[j].common.active == TRUE)
            {
                return j;
            }
        }
        else
        {
            show = &gSaveBlock1Ptr->tvShows[j];
            if (show->massOutbreak.var16 == 0 && show->massOutbreak.active == TRUE)
            {
                return j;
            }
        }
        if (j == 0)
        {
            j = ARRAY_COUNT(gSaveBlock1Ptr->tvShows) - 2;
        }
        else
        {
            j --;
        }
    } while (j != selIdx);
    return 0xFF;
}

static u8 FindAnyTVShowOnTheAir(void)
{
    u8 response;

    response = special_0x44();
    if (response == 0xFF)
    {
        return 0xFF;
    }
    if (gSaveBlock1Ptr->outbreakPokemonSpecies != SPECIES_NONE && gSaveBlock1Ptr->tvShows[response].common.kind == TVSHOW_MASS_OUTBREAK)
    {
        return FindFirstActiveTVShowThatIsNotAMassOutbreak();
    }
    return response;
}

void UpdateTVScreensOnMap(int width, int height)
{
    FlagSet(SYS_TV_WATCH);
    switch (CheckForBigMovieOrEmergencyNewsOnTV())
    {
        case 1:
            SetTVMetatilesOnMap(width, height, 0x3);
            break;
        case 2:
            break;
        default:
            if (gSaveBlock1Ptr->location.mapGroup == 0x0d && gSaveBlock1Ptr->location.mapNum == 0x00)
            {
                SetTVMetatilesOnMap(width, height, 0x3);
            }
            else if (FlagGet(SYS_TV_START) && (FindAnyTVShowOnTheAir() != 0xff || FindAnyTVNewsOnTheAir() != 0xff || IsTVShowInSearchOfTrainersAiring()))
            {
                FlagReset(SYS_TV_WATCH);
                SetTVMetatilesOnMap(width, height, 0x3);
            }
            break;
    }
}

static void SetTVMetatilesOnMap(int width, int height, u16 tileId)
{
    int x;
    int y;

    for (y = 0; y < height; y ++)
    {
        for (x = 0; x < width; x ++)
        {
            if (MapGridGetMetatileBehaviorAt(x, y) == 0x86) // is this tile a TV?
            {
                MapGridSetMetatileIdAt(x, y, tileId | 0xc00);
            }
        }
    }
}

void TurnOffTVScreen(void)
{
    SetTVMetatilesOnMap(gUnknown_03005DC0.width, gUnknown_03005DC0.height, 0x0002);
    DrawWholeMapView();
}

void TurnOnTVScreen(void)
{
    SetTVMetatilesOnMap(gUnknown_03005DC0.width, gUnknown_03005DC0.height, 0x0003);
    DrawWholeMapView();
}

u8 special_0x45(void)
{
    return gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004].common.kind;
}

static u8 FindFirstActiveTVShowThatIsNotAMassOutbreak(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(gSaveBlock1Ptr->tvShows) - 1; i ++)
    {
        if (gSaveBlock1Ptr->tvShows[i].common.kind != 0 && gSaveBlock1Ptr->tvShows[i].common.kind != TVSHOW_MASS_OUTBREAK && gSaveBlock1Ptr->tvShows[i].common.active == TRUE)
        {
            return i;
        }
    }
    return 0xFF;
}

u8 special_0x4a(void)
{
    TVShow *tvShow;

    tvShow = &gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004];
    if (tvShow->common.kind == TVSHOW_MASS_OUTBREAK && gSaveBlock1Ptr->outbreakPokemonSpecies != SPECIES_NONE)
    {
        return FindFirstActiveTVShowThatIsNotAMassOutbreak();
    }
    return gSpecialVar_0x8004;
}

// IN SEARCH OF TRAINERS

void ResetGabbyAndTy(void)
{
    gSaveBlock1Ptr->gabbyAndTyData.mon1 = SPECIES_NONE;
    gSaveBlock1Ptr->gabbyAndTyData.mon2 = SPECIES_NONE;
    gSaveBlock1Ptr->gabbyAndTyData.lastMove = MOVE_NONE;
    gSaveBlock1Ptr->gabbyAndTyData.quote[0] = -1;
    gSaveBlock1Ptr->gabbyAndTyData.valA_0 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.not_total_victory = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valA_2 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valA_3 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.onAir = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valA_5 = 0;
    gSaveBlock1Ptr->gabbyAndTyData.valB_0 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valB_1 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valB_2 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valB_3 = FALSE;
    gSaveBlock1Ptr->gabbyAndTyData.valB_4 = 0;
    gSaveBlock1Ptr->gabbyAndTyData.mapnum = 0;
    gSaveBlock1Ptr->gabbyAndTyData.battleNum = 0;
}

void GabbyAndTyBeforeInterview(void)
{
    u8 i;

    gSaveBlock1Ptr->gabbyAndTyData.mon1 = gBattleResults.poke1Species;
    gSaveBlock1Ptr->gabbyAndTyData.mon2 = gBattleResults.opponentSpecies;
    gSaveBlock1Ptr->gabbyAndTyData.lastMove = gBattleResults.lastUsedMove;
    if (gSaveBlock1Ptr->gabbyAndTyData.battleNum != 0xFF)
    {
        gSaveBlock1Ptr->gabbyAndTyData.battleNum ++;
    }
    gSaveBlock1Ptr->gabbyAndTyData.valA_0 = gBattleResults.unk5_0;
    if (gBattleResults.playerFaintCounter != 0)
    {
        gSaveBlock1Ptr->gabbyAndTyData.not_total_victory = TRUE;
    }
    else
    {
        gSaveBlock1Ptr->gabbyAndTyData.not_total_victory = FALSE;
    }
    if (gBattleResults.unk3 != 0)
    {
        gSaveBlock1Ptr->gabbyAndTyData.valA_2 = TRUE;
    }
    else
    {
        gSaveBlock1Ptr->gabbyAndTyData.valA_2 = FALSE;
    }
    if (!gBattleResults.usedMasterBall)
    {
        for (i = 0; i < 11; i ++)
        {
            if (gBattleResults.unk36[i])
            {
                gSaveBlock1Ptr->gabbyAndTyData.valA_3 = TRUE;
                break;
            }
        }
    }
    else
    {
        gSaveBlock1Ptr->gabbyAndTyData.valA_3 = TRUE;
    }
    TakeTVShowInSearchOfTrainersOffTheAir();
    if (gSaveBlock1Ptr->gabbyAndTyData.lastMove == MOVE_NONE)
    {
        FlagSet(0x0001);
    }
}

void GabbyAndTyAfterInterview(void)
{
    gSaveBlock1Ptr->gabbyAndTyData.valB_0 = gSaveBlock1Ptr->gabbyAndTyData.valA_0;
    gSaveBlock1Ptr->gabbyAndTyData.valB_1 = gSaveBlock1Ptr->gabbyAndTyData.not_total_victory;
    gSaveBlock1Ptr->gabbyAndTyData.valB_2 = gSaveBlock1Ptr->gabbyAndTyData.valA_2;
    gSaveBlock1Ptr->gabbyAndTyData.valB_3 = gSaveBlock1Ptr->gabbyAndTyData.valA_3;
    gSaveBlock1Ptr->gabbyAndTyData.onAir = TRUE;
    gSaveBlock1Ptr->gabbyAndTyData.mapnum = gMapHeader.regionMapSectionId;
    IncrementGameStat(GAME_STAT_GOT_INTERVIEWED);
}

void TakeTVShowInSearchOfTrainersOffTheAir(void)
{
    gSaveBlock1Ptr->gabbyAndTyData.onAir = FALSE;
}

u8 GabbyAndTyGetBattleNum(void)
{
    if (gSaveBlock1Ptr->gabbyAndTyData.battleNum > 5)
    {
        return (gSaveBlock1Ptr->gabbyAndTyData.battleNum % 3) + 6;
    }
    return gSaveBlock1Ptr->gabbyAndTyData.battleNum;
}

static bool8 IsTVShowInSearchOfTrainersAiring(void)
{
    return gSaveBlock1Ptr->gabbyAndTyData.onAir;
}

bool8 GabbyAndTyGetLastQuote(void)
{
    if (gSaveBlock1Ptr->gabbyAndTyData.quote[0] == 0xFFFF)
    {
        return FALSE;
    }
    CopyEasyChatWord(gStringVar1, gSaveBlock1Ptr->gabbyAndTyData.quote[0]);
    gSaveBlock1Ptr->gabbyAndTyData.quote[0] = -1;
    return TRUE;
}

u8 GabbyAndTyGetLastBattleTrivia(void)
{
    if (!gSaveBlock1Ptr->gabbyAndTyData.valB_0)
    {
        return 1;
    }
    if (gSaveBlock1Ptr->gabbyAndTyData.valB_3)
    {
        return 2;
    }
    if (gSaveBlock1Ptr->gabbyAndTyData.valB_2)
    {
        return 3;
    }
    if (gSaveBlock1Ptr->gabbyAndTyData.valB_1)
    {
        return 4;
    }
    return 0;
}

void GabbyAndTySetScriptVarsToFieldObjectLocalIds(void)
{
    switch (GabbyAndTyGetBattleNum())
    {
        case 1:
            gSpecialVar_0x8004 = 14;
            gSpecialVar_0x8005 = 13;
            break;
        case 2:
            gSpecialVar_0x8004 = 5;
            gSpecialVar_0x8005 = 6;
            break;
        case 3:
            gSpecialVar_0x8004 = 18;
            gSpecialVar_0x8005 = 17;
            break;
        case 4:
            gSpecialVar_0x8004 = 21;
            gSpecialVar_0x8005 = 22;
            break;
        case 5:
            gSpecialVar_0x8004 = 8;
            gSpecialVar_0x8005 = 9;
            break;
        case 6:
            gSpecialVar_0x8004 = 19;
            gSpecialVar_0x8005 = 20;
            break;
        case 7:
            gSpecialVar_0x8004 = 23;
            gSpecialVar_0x8005 = 24;
            break;
        case 8:
            gSpecialVar_0x8004 = 10;
            gSpecialVar_0x8005 = 11;
            break;
    }
}

void InterviewAfter(void)
{
    void sub_80ED548(void);
    void sub_80ED5B8(void);
    void sub_80ED610(void);
    void TaskDummy5(void);
    void sub_80ECF5C(void);
    void sub_80ED164(void);
    void sub_80ECA38(void);

    switch (gSpecialVar_0x8005)
    {
        case TVSHOW_FAN_CLUB_LETTER:
            sub_80ED548();
            break;
        case TVSHOW_RECENT_HAPPENINGS:
            sub_80ED5B8();
            break;
        case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
            sub_80ED610();
            break;
        case TVSHOW_UNKN_SHOWTYPE_04:
            TaskDummy5();
            break;
        case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
            sub_80ECF5C();
            break;
        case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
            sub_80ED164();
            break;
        case TVSHOW_CONTEST_LIVE_UPDATES:
            sub_80ECA38();
            break;
    }
}

void PutPokemonTodayCaughtOnAir(void)
{
    u8 i;
    u16 ct;
    TVShow *show;
    u32 language2;
    u16 itemLastUsed;

    ct = 0;
    sub_80EED88();
    sub_80ED718();
    if (gBattleResults.caughtPoke == SPECIES_NONE)
    {
        sub_80EC8FC();
    }
    else
    {
        sub_80EC8A4();
        if (!sub_80EFB38(-1) && StringCompare(gSpeciesNames[gBattleResults.caughtPoke], gBattleResults.caughtNick))
        {
            gUnknown_030060BC = sub_80EFB08(gSaveBlock1Ptr->tvShows);
            if (gUnknown_030060BC != -1 && sub_80EF46C(TVSHOW_POKEMON_TODAY_CAUGHT, 0) != TRUE)
            {
                for (i = 0; i < 11; i ++)
                {
                    ct += gBattleResults.unk36[i];
                }
                if (ct != 0 || gBattleResults.usedMasterBall)
                {
                    ct = 0;
                    show = &gSaveBlock1Ptr->tvShows[gUnknown_030060BC];
                    show->pokemonToday.kind = TVSHOW_POKEMON_TODAY_CAUGHT;
                    show->pokemonToday.active = FALSE;
                    if (gBattleResults.usedMasterBall)
                    {
                        ct = 1;
                        itemLastUsed = ITEM_MASTER_BALL;
                    }
                    else
                    {
                        for (i = 0; i < 11; i ++)
                        {
                            ct += gBattleResults.unk36[i];
                        }
                        if (ct > 0xFF)
                        {
                            ct = 0xFF;
                        }
                        itemLastUsed = gLastUsedItem;
                    }
                    show->pokemonToday.var12 = ct;
                    show->pokemonToday.ball = itemLastUsed;
                    StringCopy(show->pokemonToday.playerName, gSaveBlock2Ptr->playerName);
                    StringCopy(show->pokemonToday.nickname, gBattleResults.caughtNick);
                    language2 = sub_81DB604(show->pokemonToday.nickname);
                    StripExtCtrlCodes(show->pokemonToday.nickname);
                    show->pokemonToday.species = gBattleResults.caughtPoke;
                    sub_80EC9E8(show);
                    show->pokemonToday.language = gGameLanguage;
                    show->pokemonToday.language2 = language2;
                }
            }
        }
    }
}

asm(".section .text.dotvshow");

void TVShowDone(void);

void DoTVShow(void)
{
    void DoTVShowPokemonFanClubLetter(void);
    void DoTVShowRecentHappenings(void);
    void DoTVShowPokemonFanClubOpinions(void);
    void DoTVShowDummiedOut(void);
    void DoTVShowPokemonNewsMassOutbreak(void);
    void DoTVShowBravoTrainerPokemonProfile(void);
    void DoTVShowBravoTrainerBattleTower(void);
    void DoTVShowPokemonTodaySuccessfulCapture(void);
    void DoTVShowTodaysSmartShopper(void);
    void DoTVShowTheNameRaterShow(void);
    void DoTVShowPokemonContestLiveUpdates(void);
    void DoTVShowPokemonBattleUpdate(void);
    void DoTVShow3CheersForPokeblocks(void);
    void DoTVShowPokemonTodayFailedCapture(void);
    void DoTVShowPokemonAngler(void);
    void DoTVShowTheWorldOfMasters(void);
    void DoTVShowTodaysRivalTrainer(void);
    void DoTVShowDewfordTrendWatcherNetwork(void);
    void DoTVShowHoennTreasureInvestigators(void);
    void DoTVShowFindThatGamer(void);
    void DoTVShowBreakingNewsTV(void);
    void DoTVShowSecretBaseVisit(void);
    void DoTVShowPokemonLotterWinnerFlashReport(void);
    void DoTVShowThePokemonBattleSeminar(void);
    void DoTVShowTrainerFanClubSpecial(void);
    void DoTVShowTrainerFanClub(void);
    void DoTVShowSpotTheCuties(void);
    void DoTVShowPokemonNewsBattleFrontier(void);
    void DoTVShowWhatsNo1InHoennToday(void);
    void DoTVShowSecretBaseSecrets(void);
    void DoTVShowSafariFanClub(void);
    void DoTVShowPokemonContestLiveUpdates2(void);

    if (gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004].common.active)
    {
        switch (gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004].common.kind)
        {
            case TVSHOW_FAN_CLUB_LETTER:
                DoTVShowPokemonFanClubLetter();
                break;
            case TVSHOW_RECENT_HAPPENINGS:
                DoTVShowRecentHappenings();
                break;
            case TVSHOW_PKMN_FAN_CLUB_OPINIONS:
                DoTVShowPokemonFanClubOpinions();
                break;
            case TVSHOW_UNKN_SHOWTYPE_04:
                DoTVShowDummiedOut();
                break;
            case TVSHOW_MASS_OUTBREAK:
                DoTVShowPokemonNewsMassOutbreak();
                break;
            case TVSHOW_BRAVO_TRAINER_POKEMON_PROFILE:
                DoTVShowBravoTrainerPokemonProfile();
                break;
            case TVSHOW_BRAVO_TRAINER_BATTLE_TOWER_PROFILE:
                DoTVShowBravoTrainerBattleTower();
                break;
            case TVSHOW_POKEMON_TODAY_CAUGHT:
                DoTVShowPokemonTodaySuccessfulCapture();
                break;
            case TVSHOW_SMART_SHOPPER:
                DoTVShowTodaysSmartShopper();
                break;
            case TVSHOW_NAME_RATER_SHOW:
                DoTVShowTheNameRaterShow();
                break;
            case TVSHOW_CONTEST_LIVE_UPDATES:
                DoTVShowPokemonContestLiveUpdates();
                break;
            case TVSHOW_BATTLE_UPDATE:
                DoTVShowPokemonBattleUpdate();
                break;
            case TVSHOW_3_CHEERS_FOR_POKEBLOCKS:
                DoTVShow3CheersForPokeblocks();
                break;
            case TVSHOW_POKEMON_TODAY_FAILED:
                DoTVShowPokemonTodayFailedCapture();
                break;
            case TVSHOW_FISHING_ADVICE:
                DoTVShowPokemonAngler();
                break;
            case TVSHOW_WORLD_OF_MASTERS:
                DoTVShowTheWorldOfMasters();
                break;
            case TVSHOW_TODAYS_RIVAL_TRAINER:
                DoTVShowTodaysRivalTrainer();
                break;
            case TVSHOW_TREND_WATCHER:
                DoTVShowDewfordTrendWatcherNetwork();
                break;
            case TVSHOW_TREASURE_INVESTIGATORS:
                DoTVShowHoennTreasureInvestigators();
                break;
            case TVSHOW_FIND_THAT_GAMER:
                DoTVShowFindThatGamer();
                break;
            case TVSHOW_BREAKING_NEWS:
                DoTVShowBreakingNewsTV();
                break;
            case TVSHOW_SECRET_BASE_VISIT:
                DoTVShowSecretBaseVisit();
                break;
            case TVSHOW_LOTTO_WINNER:
                DoTVShowPokemonLotterWinnerFlashReport();
                break;
            case TVSHOW_BATTLE_SEMINAR:
                DoTVShowThePokemonBattleSeminar();
                break;
            case TVSHOW_FAN_CLUB_SPECIAL:
                DoTVShowTrainerFanClubSpecial();
                break;
            case TVSHOW_TRAINER_FAN_CLUB:
                DoTVShowTrainerFanClub();
                break;
            case TVSHOW_CUTIES:
                DoTVShowSpotTheCuties();
                break;
            case TVSHOW_FRONTIER:
                DoTVShowPokemonNewsBattleFrontier();
                break;
            case TVSHOW_NUMBER_ONE:
                DoTVShowWhatsNo1InHoennToday();
                break;
            case TVSHOW_SECRET_BASE_SECRETS:
                DoTVShowSecretBaseSecrets();
                break;
            case TVSHOW_SAFARI_FAN_CLUB:
                DoTVShowSafariFanClub();
                break;
            case TVSHOW_CONTEST_LIVE_UPDATES_2:
                DoTVShowPokemonContestLiveUpdates2();
                break;
        }
    }
}

void DoTVShowBravoTrainerPokemonProfile(void)
{
    TVShow *show;
    u8 state;

    show = &gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004];
    gScriptResult = 0;
    state = sTVShowState;
    switch (state)
    {
        case 0:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            CopyContestCategoryToStringVar(1, show->bravoTrainer.contestCategory);
            CopyContestRankToStringVar(2, show->bravoTrainer.contestRank);
            if (!StringCompare(gSpeciesNames[show->bravoTrainer.species], show->bravoTrainer.pokemonNickname))
                sTVShowState = 8;
            else
                sTVShowState = 1;
            break;
        case 1:
            StringCopy(gStringVar1, gSpeciesNames[show->bravoTrainer.species]);
            TVShowConvertInternationalString(gStringVar2, show->bravoTrainer.pokemonNickname, show->bravoTrainer.pokemonNameLanguage);
            CopyContestCategoryToStringVar(2, show->bravoTrainer.contestCategory);
            sTVShowState = 2;
            break;
        case 2:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            if (show->bravoTrainer.contestResult == 0) // placed first
                sTVShowState = 3;
            else
                sTVShowState = 4;
            break;
        case 3:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            CopyEasyChatWord(gStringVar2, show->bravoTrainer.ecWords[0]);
            TV_ConvertNumberToOrdinal(2, show->bravoTrainer.contestResult + 1);
            sTVShowState = 5;
            break;
        case 4:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            CopyEasyChatWord(gStringVar2, show->bravoTrainer.ecWords[0]);
            TV_ConvertNumberToOrdinal(2, show->bravoTrainer.contestResult + 1);
            sTVShowState = 5;
            break;
        case 5:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            CopyContestCategoryToStringVar(1, show->bravoTrainer.contestCategory);
            CopyEasyChatWord(gStringVar3, show->bravoTrainer.ecWords[1]);
            if (show->bravoTrainer.move)
                sTVShowState = 6;
            else
                sTVShowState = 7;
            break;
        case 6:
            StringCopy(gStringVar1, gSpeciesNames[show->bravoTrainer.species]);
            StringCopy(gStringVar2, gMoveNames[show->bravoTrainer.move]);
            CopyEasyChatWord(gStringVar3, show->bravoTrainer.ecWords[1]);
            sTVShowState = 7;
            break;
        case 7:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainer.playerName, show->bravoTrainer.language);
            StringCopy(gStringVar2, gSpeciesNames[show->bravoTrainer.species]);
            TVShowDone();
            break;
        case 8:
            StringCopy(gStringVar1, gSpeciesNames[show->bravoTrainer.species]);
            sTVShowState = 2;
            break;
    }
    ShowFieldMessage(gTVBravoTrainerTextGroup[state]);
}

void DoTVShowBravoTrainerBattleTower(void)
{
    TVShow *show;
    u8 state;

    show = &gSaveBlock1Ptr->tvShows[gSpecialVar_0x8004];
    gScriptResult = 0;
    state = sTVShowState;
    switch(state)
    {
        case 0:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.trainerName, show->bravoTrainerTower.language);
            StringCopy(gStringVar2, gSpeciesNames[show->bravoTrainerTower.species]);
            if (show->bravoTrainerTower.numFights >= 7)
                sTVShowState = 1;
            else
                sTVShowState = 2;
            break;
        case 1:
            if (show->bravoTrainerTower.btLevel == 50)
            {
                StringCopy(gStringVar1, gText_Lv50);
            }
            else
            {
                StringCopy(gStringVar1, gText_OpenLevel);
            }
            TV_ConvertNumberToOrdinal(1, show->bravoTrainerTower.numFights);
            if (show->bravoTrainerTower.var1c == 1)
                sTVShowState = 3;
            else
                sTVShowState = 4;
            break;
        case 2:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            TV_ConvertNumberToOrdinal(1, show->bravoTrainerTower.numFights + 1);
            if (show->bravoTrainerTower.var1b == 0)
                sTVShowState = 5;
            else
                sTVShowState = 6;
            break;
        case 3:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            StringCopy(gStringVar2, gSpeciesNames[show->bravoTrainerTower.defeatedSpecies]);
            if (show->bravoTrainerTower.var1b == 0)
                sTVShowState = 5;
            else
                sTVShowState = 6;
            break;
        case 4:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            StringCopy(gStringVar2, gSpeciesNames[show->bravoTrainerTower.defeatedSpecies]);
            if (show->bravoTrainerTower.var1b == 0)
                sTVShowState = 5;
            else
                sTVShowState = 6;
            break;
        case 5:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            sTVShowState = 11;
            break;
        case 6:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            sTVShowState = 11;
            break;
        case 7:
            sTVShowState = 11;
            break;
        case 8:
        case 9:
        case 10:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.trainerName, show->bravoTrainerTower.language);
            sTVShowState = 11;
            break;
        case 11:
            CopyEasyChatWord(gStringVar1, show->bravoTrainerTower.var18[0]);
            if (show->bravoTrainerTower.var1b == 0)
                sTVShowState = 12;
            else
                sTVShowState = 13;
            break;
        case 12:
        case 13:
            CopyEasyChatWord(gStringVar1, show->bravoTrainerTower.var18[0]);
            TVShowConvertInternationalString(gStringVar2, show->bravoTrainerTower.trainerName, show->bravoTrainerTower.language);
            TVShowConvertInternationalString(gStringVar3, show->bravoTrainerTower.pokemonName, show->bravoTrainerTower.pokemonNameLanguage);
            sTVShowState = 14;
            break;
        case 14:
            TVShowConvertInternationalString(gStringVar1, show->bravoTrainerTower.trainerName, show->bravoTrainerTower.language);
            StringCopy(gStringVar2, gSpeciesNames[show->bravoTrainerTower.species]);
            TVShowDone();
            break;
    }
    ShowFieldMessage(gTVBravoTrainerBattleTowerTextGroup[state]);
}
