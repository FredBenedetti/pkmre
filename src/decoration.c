
// Includes
#include "global.h"
#include "string_util.h"
#include "international_string_util.h"
#include "script.h"
#include "task.h"
#include "palette.h"
#include "songs.h"
#include "field_weather.h"
#include "menu_helpers.h"
#include "new_menu_helpers.h"
#include "sound.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "event_scripts.h"
#include "player_pc.h"
#include "strings.h"

// Static type declarations

// Static RAM declarations

extern EWRAM_DATA u8 *gUnknown_0203A14C;
extern EWRAM_DATA u8 gUnknown_0203A150;
extern EWRAM_DATA u8 gUnknown_0203A151;
extern EWRAM_DATA u16 gUnknown_0203A16E;
extern EWRAM_DATA u16 gUnknown_0203A170;
extern EWRAM_DATA u8 gUnknown_0203A173;
extern EWRAM_DATA struct DecoPCPointers gUnknown_0203A17C;
extern EWRAM_DATA u8 gUnknown_0203A188[4];
extern EWRAM_DATA struct {
    u8 filler_0000[0x520];
    u8 unk_520;
    u8 unk_521;
} *gUnknown_0203A18C;

// Static ROM declarations

void sub_8126B80(u8 taskId);
void sub_8126C08(void);
void sub_8126DA4(u8 taskId);
void sub_8126D6C(u8 taskId);
void sub_8126DCC(u8 taskId);
void sub_8126DFC(u8 taskId);
void sub_8126E8C(u8 taskId);
void sub_8126F68(u8 winid, u8 decorCat, u8 x, u8 y, bool8 flag, u8 speed);
void sub_8127058(u8 *str, bool8 flag);
void sub_8127088(u8 taskId);
void sub_81270E8(u8 taskId);
void sub_8127180(u8 taskId);
void sub_812719C(u8 taskId);
void sub_81271CC(u8 taskId);
void sub_8127268(u8 taskId);
void sub_8127620(u8 taskId);
void sub_81279B4(u8 taskId);
bool8 sub_81299AC(u8 taskId);
void sub_8129ABC(u8 taskId);
void sub_8133E1C(u8 taskId);

// .rodata

extern const struct WindowTemplate gUnknown_085A6B90[4];
extern const u8 *const gUnknown_085A6B28[];
extern const struct MenuAction gUnknown_085A6B48[];
extern const u8 *const gUnknown_085A6B68[];
extern const u16 gUnknown_085A6BB0[];

// .text

void sub_8126968(void)
{
    if (gUnknown_0203A173 < 8)
    {
        gUnknown_0203A14C = gDecorationInventories[gUnknown_0203A173].items;
    }
    if (gUnknown_0203A17C.isPlayerRoom == FALSE)
    {
        gUnknown_0203A17C.items = gSaveBlock1Ptr->secretBases[0].decorations;
        gUnknown_0203A17C.pos = gSaveBlock1Ptr->secretBases[0].decorationPos;
    }
    if (gUnknown_0203A17C.isPlayerRoom == TRUE)
    {
        gUnknown_0203A17C.items = gSaveBlock1Ptr->playerRoomDecor;
        gUnknown_0203A17C.pos = gSaveBlock1Ptr->playerRoomDecorPos;
    }
}

u8 sub_81269D4(u8 idx)
{
    u8 *winidx;
    struct WindowTemplate template;

    winidx = &gUnknown_0203A188[idx];
    if (idx == 0)
    {
        template = gUnknown_085A6B90[0];
        template.width = GetMaxWidthInMenuTable(gUnknown_085A6B48, 4);
        if (template.width > 18)
        {
            template.width = 18;
        }
        *winidx = AddWindow(&template);
    }
    else
    {
        *winidx = AddWindow(&gUnknown_085A6B90[idx]);
    }
    SetWindowBorderStyle(*winidx, 0, 0x214, 0xe);
    schedule_bg_copy_tilemap_to_vram(0);
    return *winidx;
}

void sub_8126A58(u8 idx)
{
    sub_8198070(gUnknown_0203A188[idx], FALSE);
    ClearWindowTilemap(gUnknown_0203A188[idx]);
    RemoveWindow(gUnknown_0203A188[idx]);
    schedule_bg_copy_tilemap_to_vram(0);
}

void sub_8126A88(void)
{
    u8 idx;

    idx = sub_81269D4(0);
    PrintMenuTable(idx, 4, gUnknown_085A6B48);
    InitMenuInUpperLeftCornerPlaySoundWhenAPressed(idx, 4, gUnknown_0203A150);
}

void sub_8126ABC(void)
{
    gUnknown_0203A150 = 0;
    ScriptContext2_Enable();
    sub_8126A88();
    sub_8126C08();
}

void sub_8126AD8(u8 taskId)
{
    sub_8126ABC();
    gUnknown_0203A17C.items = gSaveBlock1Ptr->secretBases[0].decorations;
    gUnknown_0203A17C.pos = gSaveBlock1Ptr->secretBases[0].decorationPos;
    gUnknown_0203A17C.size = sizeof(gSaveBlock1Ptr->secretBases[0].decorations);
    gUnknown_0203A17C.isPlayerRoom = FALSE;
    gTasks[taskId].func = sub_8126B80;
}

void sub_8126B2C(u8 taskId)
{
    sub_8126ABC();
    gUnknown_0203A17C.items = gSaveBlock1Ptr->playerRoomDecor;
    gUnknown_0203A17C.pos = gSaveBlock1Ptr->playerRoomDecorPos;
    gUnknown_0203A17C.size = sizeof(gSaveBlock1Ptr->playerRoomDecor);
    gUnknown_0203A17C.isPlayerRoom = TRUE;
    gTasks[taskId].func = sub_8126B80;
}

void sub_8126B80(u8 taskId)
{
    u8 menuPos;

    if (!gPaletteFade.active)
    {
        menuPos = GetMenuCursorPos();
        switch (ProcessMenuInput())
        {
            default:
                PlaySE(SE_SELECT);
                gUnknown_085A6B48[gUnknown_0203A150].func.void_u8(taskId);
                break;
            case -2:
                gUnknown_0203A150 = GetMenuCursorPos();
                if ((s8)menuPos != gUnknown_0203A150)
                {
                    sub_8126C08();
                }
                break;
            case -1:
                PlaySE(SE_SELECT);
                sub_8126D6C(taskId);
                break;
        }
    }
}

void sub_8126C08(void)
{
    FillWindowPixelBuffer(0, 0x11);
    AddTextPrinterParametrized(0, 1, gUnknown_085A6B68[gUnknown_0203A150], 0, 0, 2, 1, 3);
}

void sub_8126C48(u8 taskId)
{
    if (CountDecorations() == 0)
    {
        StringExpandPlaceholders(gStringVar4, gText_NoDecorations);
        DisplayItemMessageOnField(taskId, gStringVar4, sub_8126DA4);
    }
    else
    {
        gTasks[taskId].data[11] = 0;
        gUnknown_0203A173 = DECORCAT_DESK;
        sub_8126DCC(taskId);
    }
}

void sub_8126CA4(u8 taskId)
{
    if (!sub_81299AC(taskId))
    {
        StringExpandPlaceholders(gStringVar4, gText_NoDecorationsInUse);
        DisplayItemMessageOnField(taskId, gStringVar4, sub_8126DA4);
    }
    else
    {
        sub_8126A58(0);
        sub_8197434(0, 0);
        fade_screen(1, 0);
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].func = sub_8129ABC;
    }
}

void sub_8126D10(u8 taskId)
{
    if (CountDecorations() == 0)
    {
        StringExpandPlaceholders(gStringVar4, gText_NoDecorations);
        DisplayItemMessageOnField(taskId, gStringVar4, sub_8126DA4);
    }
    else
    {
        gTasks[taskId].data[11] = 1;
        gUnknown_0203A173 = DECORCAT_DESK;
        sub_8126DCC(taskId);
    }
}

void sub_8126D6C(u8 taskId)
{
    sub_8126A58(0);
    if (!gUnknown_0203A17C.isPlayerRoom)
    {
        ScriptContext1_SetupScript(gUnknown_0823B4E8);
        DestroyTask(taskId);
    }
    else
    {
        sub_816B060(taskId);
    }
}

void sub_8126DA4(u8 taskId)
{
    sub_8126C08();
    gTasks[taskId].func = sub_8126B80;
}

void sub_8126DCC(u8 taskId)
{
    LoadPalette(gUnknown_085A6BB0, 0xd0, 0x20);
    sub_8197434(0, 0);
    sub_8126A58(0);
    sub_8126DFC(taskId);
}

void sub_8126DFC(u8 taskId)
{
    u8 winIdx;

    winIdx = sub_81269D4(1);
    sub_8126E8C(taskId);
    InitMenuInUpperLeftCornerPlaySoundWhenAPressed(winIdx, 9, gUnknown_0203A173);
    gTasks[taskId].func = sub_8127088;
}

void sub_8126E44(u8 taskId)
{
    FillWindowPixelBuffer(gUnknown_0203A188[1], 0x11);
    sub_8126E8C(taskId);
    InitMenuInUpperLeftCornerPlaySoundWhenAPressed(gUnknown_0203A188[1], 9, gUnknown_0203A173);
    gTasks[taskId].func = sub_8127088;
}

void sub_8126E8C(u8 taskId)
{
    s16 *data;
    u8 r5;
    bool8 r8;
    u8 i;
    bool8 fl;

    data = gTasks[taskId].data;
    r5 = gUnknown_0203A188[1];
    fl = gUnknown_0203A17C.isPlayerRoom;
    r8 = FALSE;
    if (fl == TRUE && data[11] == 0)
    {
        r8 = TRUE;
    }
    for (i = 0; i < 8; i ++)
    {
        if (r8 == TRUE && i != DECORCAT_DOLL && i != DECORCAT_CUSHION)
        {
            sub_8126F68(r5, i, 8, i << 4, TRUE, 0xFF);
        }
        else
        {
            sub_8126F68(r5, i, 8, i << 4, FALSE, 0xFF);
        }
    }
    PrintTextOnWindow(r5, 1, gTasks[taskId].data[11] == 2 ? gText_Exit : gText_Cancel, 8, (i << 4) + 1, 0, 0);
    schedule_bg_copy_tilemap_to_vram(0);
}

void sub_8126F68(u8 winid, u8 decorCat, u8 x, u8 y, bool8 flag, u8 speed)
{
    u8 width;
    u8 *strbuf;

    width = x == 8 ? 0x68 : 0x60;
    y ++;
    sub_8127058(gStringVar4, flag);
    strbuf = StringLength(gStringVar4) + gStringVar4;
    StringCopy(strbuf, gUnknown_085A6B28[decorCat]);
    PrintTextOnWindow(winid, 1, gStringVar4, x, y, speed, NULL);
    strbuf = ConvertIntToDecimalStringN(strbuf, CountDecorationCategoryN(decorCat), STR_CONV_MODE_RIGHT_ALIGN, 2);
    *strbuf++ = CHAR_SLASH;
    ConvertIntToDecimalStringN(strbuf, gDecorationInventories[decorCat].size, STR_CONV_MODE_RIGHT_ALIGN, 2);
    x = GetStringRightAlignXOffset(1, gStringVar4, width);
    PrintTextOnWindow(winid, 1, gStringVar4, x, y, speed, NULL);
}

void sub_8127058(u8 *str, bool8 flag)
{
    StringCopy(str, gText_Color161Shadow161);
    if (flag == TRUE)
    {
        str[2] = 0x04; // RED
        str[5] = 0x05; // LIGHT_RED
    }
    else
    {
        str[2] = 0x02; // DARK_GREY
        str[5] = 0x03; // LIGHT_GREY
    }
}

void sub_8127088(u8 taskId)
{
    s8 input;

    if (!gPaletteFade.active)
    {
        input = ProcessMenuInput();
        switch (input)
        {
            case -1:
            case 8:
                PlaySE(SE_SELECT);
                sub_812719C(taskId);
                break;
            case -2:
                break;
            default:
                PlaySE(SE_SELECT);
                gUnknown_0203A173 = input;
                sub_81270E8(taskId);
                break;
        }
    }
}

void sub_81270E8(u8 taskId)
{
    gUnknown_0203A151 = CountDecorationCategoryN(gUnknown_0203A173);
    if (gUnknown_0203A151 != 0)
    {
        CondenseDecorationCategoryN(gUnknown_0203A173);
        gUnknown_0203A14C = gDecorationInventories[gUnknown_0203A173].items;
        sub_81279B4(taskId);
        gUnknown_0203A170 = 0;
        gUnknown_0203A16E = 0;
        gTasks[taskId].func = sub_8127620;
    }
    else
    {
        sub_8126A58(1);
        StringExpandPlaceholders(gStringVar4, gText_NoDecorations);
        DisplayItemMessageOnField(taskId, gStringVar4, sub_8127180);
    }
}

void sub_8127180(u8 taskId)
{
    sub_8197434(0, 0);
    sub_8126DFC(taskId);
}

void sub_812719C(u8 taskId)
{
    if (gTasks[taskId].data[11] != 2)
    {
        sub_81271CC(taskId);
    }
    else
    {
        sub_8127268(taskId);
    }
}

void sub_81271CC(u8 taskId)
{
    sub_8126A58(1);
    sub_8126A88();
    sub_81973C4(0, 0);
    sub_8126C08();
    gTasks[taskId].func = sub_8126B80;
}

void sub_8127208(u8 taskId)
{
    LoadPalette(gUnknown_085A6BB0, 0xd0, 0x20);
    sub_8197434(0, 0);
    gTasks[taskId].data[11] = 2;
    gUnknown_0203A173 = DECORCAT_DESK;
    sub_8126DFC(taskId);
}

void sub_8127250(u8 *dest, u8 decorCat)
{
    StringCopy(dest, gUnknown_085A6B28[decorCat]);
}

void sub_8127268(u8 taskId)
{
    sub_8126A58(1);
    sub_8133E1C(taskId);
}

void sub_8127284(void)
{
    gUnknown_0203A18C->unk_520 = gUnknown_0203A151 + 1;
    if (gUnknown_0203A18C->unk_520 > 8)
    {
        gUnknown_0203A18C->unk_521 = 8;
    }
    else
    {
        gUnknown_0203A18C->unk_521 = gUnknown_0203A18C->unk_520;
    }
}

void sub_81272C8(void)
{
    sub_812225C(&gUnknown_0203A170, &gUnknown_0203A16E, gUnknown_0203A18C->unk_521, gUnknown_0203A18C->unk_520);
}

void sub_81272F8(void)
{
    sub_8122298(&gUnknown_0203A170, &gUnknown_0203A16E, gUnknown_0203A18C->unk_521, gUnknown_0203A18C->unk_520, 8);
}
