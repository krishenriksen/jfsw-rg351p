//-------------------------------------------------------------------------
/*
Copyright (C) 1997, 2005 - 3D Realms Entertainment

This file is part of Shadow Warrior version 1.2

Shadow Warrior is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1997 - Frank Maddin and Jim Norwood
Prepared for public release: 03/28/2005 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

// _functio.h

// file created by makehead.exe
// these headers contain default key assignments, as well as
// default button assignments and game function names
// axis defaults are also included


#ifndef _function_private_
#define _function_private_
char * gamefunctions[] =
   {
   "Move_Forward",
   "Move_Backward",
   "Turn_Left",
   "Turn_Right",
   "Strafe",
   "Fire",
   "Open",
   "Run",
   "AutoRun",
   "Jump",
   "Crouch",
   "Look_Up",
   "Look_Down",
   "Strafe_Left",
   "Strafe_Right",
   "Aim_Up",
   "Aim_Down",
   "Weapon_1",
   "Weapon_2",
   "Weapon_3",
   "Weapon_4",
   "Weapon_5",
   "Weapon_6",
   "Weapon_7",
   "Weapon_8",
   "Weapon_9",
   "Weapon_10",
   "Inventory",
   "Inventory_Left",
   "Inventory_Right",
   "Med_Kit",
   "Smoke_Bomb",
   "Night_Vision",
   "Gas_Bomb",
   "Flash_Bomb",
   "Caltrops",
   "TurnAround",
   "SendMessage",
   "Map",
   "Shrink_Screen",
   "Enlarge_Screen",
   "Center_View",
   "Holster_Weapon",
   "Map_Follow_Mode",
   "See_Co_Op_View",
   "Mouse_Aiming",
   "Toggle_Crosshair",
   "Next_Weapon",
   "Previous_Weapon",
   "Show_Menu",
   "Show_Console",
   };

#define NUMKEYENTRIES 50

static char * keydefaults[] =
   {
   "Move_Forward", "Up", "Kpad8",
   "Move_Backward", "Down", "Kpad2",
   "Turn_Left", "Left", "Kpad4",
   "Turn_Right", "Right", "KPad6",
   "Strafe", "LAlt", "RAlt",
   "Fire", "LCtrl", "RCtrl",
   "Open", "Space", "",
   "Run", "LShift", "RShift",
   "AutoRun", "CapLck", "",
   "Jump", "A", "/",
   "Crouch", "Z", "",
   "Look_Up", "PgUp", "Kpad9",
   "Look_Down", "PgDn", "Kpad3",
   "Strafe_Left", ",", "",
   "Strafe_Right", ".", "",
   "Aim_Up", "Home", "KPad7",
   "Aim_Down", "End", "Kpad1",
   "Weapon_1", "1", "",
   "Weapon_2", "2", "",
   "Weapon_3", "3", "",
   "Weapon_4", "4", "",
   "Weapon_5", "5", "",
   "Weapon_6", "6", "",
   "Weapon_7", "7", "",
   "Weapon_8", "8", "",
   "Weapon_9", "9", "",
   "Weapon_10", "0", "",
   "Inventory", "Enter", "KpdEnt",
   "Inventory_Left", "[", "",
   "Inventory_Right", "]", "",
   "Med_Kit", "M", "",
   "Smoke_Bomb", "S", "",
   "Night_Vision", "N", "",
   "Gas_Bomb", "G", "",
   "Flash_Bomb", "F", "",
   "Caltrops", "C", "",
   "TurnAround", "BakSpc", "",
   "SendMessage", "T", "",
   "Map", "Tab", "",
   "Shrink_Screen", "-", "Kpad-",
   "Enlarge_Screen", "=", "Kpad+",
   "Center_View", "KPad5", "",
   "Holster_Weapon", "ScrLck", "",
   "Map_Follow_Mode", "F", "",
   "See_Co_Op_View", "K", "",
   "Mouse_Aiming", "U", "",
   "Toggle_Crosshair", "I", "",
   "Next_Weapon", "'", "",
   "Previous_Weapon", ";", "",
   "Show_Console", "NumLck", "",
   };

static char * keydefaults_modern[] =
   {
   "Move_Forward", "W", "",
   "Move_Backward", "S", "",
   "Turn_Left", "", "",
   "Turn_Right", "", "",
   "Strafe", "", "",
   "Fire", "", "",
   "Open", "E", "",
   "Run", "LShift", "",
   "AutoRun", "CapLck", "",
   "Jump", "Space", "",
   "Crouch", "LAlt", "",
   "Look_Up", "", "",
   "Look_Down", "", "",
   "Strafe_Left", "A", "",
   "Strafe_Right", "D", "",
   "Aim_Up", "", "",
   "Aim_Down", "", "",
   "Weapon_1", "1", "",
   "Weapon_2", "2", "",
   "Weapon_3", "3", "",
   "Weapon_4", "4", "",
   "Weapon_5", "5", "",
   "Weapon_6", "6", "",
   "Weapon_7", "7", "",
   "Weapon_8", "8", "",
   "Weapon_9", "9", "",
   "Weapon_10", "0", "",
   "Inventory", "Enter", "",
   "Inventory_Left", "[", "",
   "Inventory_Right", "]", "",
   "Med_Kit", "M", "",
   "Smoke_Bomb", "B", "",
   "Night_Vision", "N", "",
   "Gas_Bomb", "G", "",
   "Flash_Bomb", "F", "",
   "Caltrops", "C", "",
   "TurnAround", "BakSpc", "",
   "SendMessage", "T", "",
   "Map", "Tab", "",
   "Shrink_Screen", "-", "",
   "Enlarge_Screen", "=", "",
   "Center_View", "", "",
   "Holster_Weapon", "H", "",
   "Map_Follow_Mode", "F", "",
   "See_Co_Op_View", "K", "",
   "Mouse_Aiming", "U", "",
   "Toggle_Crosshair", "I", "",
   "Next_Weapon", "", "",
   "Previous_Weapon", "", "",
   "Show_Console", "NumLck", "",
   };


static char * mousedefaults[MAXMOUSEBUTTONS] =
   {
   "Fire",
   "Strafe",
   "Move_Forward",
   "",
   ""
   };

static char * mousedefaults_modern[MAXMOUSEBUTTONS] =
   {
   "Fire",
   "Open",
   "",
   "",
   "Next_Weapon",
   "Previous_Weapon"
   };


static char * mouseclickeddefaults[MAXMOUSEBUTTONS] =
   {
   "",
   "Open",
   "",
   "",
   ""
   };

static char * mouseclickeddefaults_modern[MAXMOUSEBUTTONS] =
   {
   "",
   "",
   "",
   "",
   "",
   ""
   };


static char * joystickdefaults[MAXJOYBUTTONS] =
   {
   "Fire",        // A
   "Strafe",      // B
   "Run",         // X
   "Open",        // Y
   "",            // Back
   "",            // Guide
   "",            // Start
   "",            // L thumb
   "",            // R thumb
   "",            // L shoulder
   "",            // R shoulder
   "Aim_Up",      // DP up
   "Aim_Down",    // DP down
   "Look_Left",   // DP left
   "Look_Right",  // DP right
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickclickeddefaults[MAXJOYBUTTONS] =
   {
   "",            // A
   "Inventory",   // B
   "Jump",        // X
   "Crouch",      // Y
   "",            // Back
   "",            // Guide
   "",            // Start
   "",            // L thumb
   "",            // R thumb
   "",            // L shoulder
   "",            // R shoulder
   "",            // DP up
   "",            // DP down
   "",            // DP left
   "",            // DP right
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickdefaults_modern[MAXJOYBUTTONS] =
   {
   "Jump",        // A
   "Crouch",      // B
   "Open",        // X
   "",            // Y
   "Map",         // Back
   "",            // Guide
   "Show_Menu",   // Start
   "",            // L thumb
   "",            // R thumb
   "Previous_Weapon", // L shoulder
   "Next_Weapon", // R shoulder
   "Inventory",   // DP up
   "Med_Kit",     // DP down
   "Inventory_Left", // DP left
   "Inventory_Right", // DP right
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickclickeddefaults_modern[MAXJOYBUTTONS] =
   {
   "",            // A
   "",            // B
   "",            // X
   "",            // Y
   "",            // Back
   "",            // Guide
   "",            // Start
   "",            // L thumb
   "",            // R thumb
   "",            // L shoulder
   "",            // R shoulder
   "",            // DP up
   "",            // DP down
   "",            // DP left
   "",            // DP right
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * mouseanalogdefaults[MAXMOUSEAXES] =
   {
   "analog_turning",
   "analog_moving",
   };


static char * mousedigitaldefaults[MAXMOUSEAXES*2] =
   {
   "",
   "",
   "",
   "",
   };


static char * joystickanalogdefaults[MAXJOYAXES] =
   {
   "analog_turning",
   "analog_moving",
   "analog_strafing",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickdigitaldefaults[MAXJOYAXES*2] =
   {
   "",
   "",
   "",
   "",
   "",
   "",
   "Run",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickanalogdefaults_modern[MAXJOYAXES] =
   {
   "analog_strafing",
   "analog_moving",
   "analog_turning",
   "analog_lookingupanddown",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };


static char * joystickdigitaldefaults_modern[MAXJOYAXES*2] =
   {
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "Fire",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   };
#endif
