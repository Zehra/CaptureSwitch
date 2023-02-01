// CaptureSwitch.cpp

#include "bzfsAPI.h"

const char* teamToFlagType(bz_eTeamType team) {
    if (team == eRedTeam) {
        return "R*";
    } else if (team == eGreenTeam) {
        return "G*";
    } else if (team == eBlueTeam) {
        return "B*";
    } else if (team == ePurpleTeam) {
        return "P*";
    } else {
        return "US";
    }
}

bz_eTeamType flagToTeamValue(const char* flagType) {
    if (strcmp("R*", flagType) == 0) {
        return eRedTeam;
    } else if (strcmp("G*", flagType) == 0) {
        return eGreenTeam;
    } else if (strcmp("B*", flagType) == 0) {
        return eBlueTeam;
    } else if (strcmp("P*", flagType) == 0) {
        return ePurpleTeam;
    }  else {
        return eNoTeam;
    }
}


class CaptureSwitch : public bz_Plugin
{
public:
  const char* Name(){return "CaptureSwitch";}
  void Init ( const char* /*config*/ );
  void Event(bz_EventData *eventData );
  void Cleanup ( void );
  int cappingPlayer = -1;
  int alive = -1;
  bz_eTeamType cappingTeam = eNoTeam;
  bz_eTeamType basePoint;
};

BZ_PLUGIN(CaptureSwitch)

void CaptureSwitch::Init (const char* commandLine) {
  Register(bz_eAllowCTFCaptureEvent);
  Register(bz_eCaptureEvent);
  Register(bz_eTickEvent);
  Register(bz_ePlayerPartEvent);
  Register(bz_ePlayerDieEvent);
  Register(bz_ePlayerSpawnEvent);
  MaxWaitTime = 0.2f;
}

void CaptureSwitch::Cleanup (void) {
  Flush();
}

void CaptureSwitch::Event(bz_EventData *eventData ){
  switch (eventData->eventType) {
    case bz_eAllowCTFCaptureEvent: {
      bz_AllowCTFCaptureEventData_V1* allowCapData = (bz_AllowCTFCaptureEventData_V1*)eventData;
      if (bz_getPlayerTeam(allowCapData->playerCapping) == allowCapData->teamCapped) {
        allowCapData->allow = false;
        basePoint = bz_checkBaseAtPoint(allowCapData->pos);
        if (basePoint != eNoTeam) {
            bz_resetFlag(bz_getPlayerFlagID(allowCapData->playerCapping));
            cappingPlayer = allowCapData->playerCapping;
            cappingTeam = bz_getPlayerTeam(allowCapData->playerCapping);
            alive = 1;
            //
            bz_givePlayerFlag(cappingPlayer, teamToFlagType(basePoint), true);
            
        }
        
        
        // This shouldn't be so tedious to do.
       
        }

// Data
// ---
// (int)          playerCapping - The ID of the player making the capture
// (bz_eTeamType) teamCapped - The team whose flag is being captured
// (bz_eTeamType) teamCapping - The team making the capture
// (float[3])     pos - The position of the player
// (float)        rot - The direction the player is
// (bool)         allow - Whether or not to allow the capture
// (bool)         killTeam - Whether or not tanks on the losing team should explode
// (double)       eventTime - The server time at which the event occurred (in seconds)
        
    }break;

    case bz_eCaptureEvent: {
      bz_CTFCaptureEventData_V1* capData = (bz_CTFCaptureEventData_V1*)eventData;
      if (capData->playerCapping == cappingPlayer) {
        if (capData->teamCapping == cappingTeam) {
            //bz_killPlayer(capData->playerCapping, true, BZ_SERVER);
            cappingPlayer = -1;
            alive = -1;
            cappingTeam = eNoTeam;
            basePoint = eNoTeam;
        }
      }

// Data
// ---
// (bz_eTeamType) teamCapped - The team whose flag was captured.
// (bz_eTeamType) teamCapping - The team who did the capturing.
// (int)          playerCapping - The player who captured the flag.
// (float[3])     pos - The world position(X,Y,Z) where the flag has been captured
// (float)        rot - The rotational orientation of the capturing player
// (double)       eventTime - This value is the local server time of the event.
        
    }break;

    case bz_eTickEvent: {
      if ((cappingPlayer != -1) && (alive == 1)) {
        int flagID = bz_getPlayerFlagID(cappingPlayer);
        if (flagID != -1) {
            bz_eTeamType flagTeam = flagToTeamValue(bz_getFlagName(flagID).c_str());
            if (flagTeam != basePoint) {
                bz_resetFlag(flagID);
                bz_givePlayerFlag(cappingPlayer, teamToFlagType(basePoint), true);
            }
        } else {
            bz_givePlayerFlag(cappingPlayer, teamToFlagType(basePoint), true);
        }
        bz_triggerFlagCapture(cappingPlayer, cappingTeam, basePoint);
      }
    }break;

    case bz_ePlayerPartEvent: {
      bz_PlayerJoinPartEventData_V1* partData = (bz_PlayerJoinPartEventData_V1*)eventData;
      int player = partData->playerID;
      if (player == cappingPlayer) {
        alive = -1;
        cappingPlayer = -1;
        cappingTeam = eNoTeam;
        basePoint = eNoTeam;
      
      }
        
    }break;

      case bz_ePlayerDieEvent: {
      //int playerID=((bz_PlayerDieEventData_V2*)eventData)->playerID;
      bz_PlayerDieEventData_V2* deathData = (bz_PlayerDieEventData_V2*)eventData;
      if ((deathData->playerID == cappingPlayer) && (cappingPlayer != -1)) {
        alive = -1;
      }

// Data
// ---
// (int)          playerID - ID of the player who was killed.
// (bz_eTeamType) team - The team the killed player was on.
// (int)          killerID - The owner of the shot that killed the player, or BZ_SERVER for server side kills
// (bz_eTeamType) killerTeam - The team the owner of the shot was on.
// (bz_ApiString) flagKilledWith - The flag name the owner of the shot had when the shot was fired.
// (int)          flagHeldWhenKilled - The ID of the flag the victim was holding when they died.
// (int)          shotID - The shot ID that killed the player, if the player was not killed by a shot, the id will be -1.
// (bz_PlayerUpdateState) state - The state record for the killed player at the time of the event
// (double)       eventTime - Time of the event on the server.
        
    }break;
     
    case bz_ePlayerSpawnEvent: {
      bz_PlayerSpawnEventData_V1* spawnData = (bz_PlayerSpawnEventData_V1*)eventData;
      if ((spawnData->playerID == cappingPlayer) && (cappingPlayer != -1)) {
        alive = 1;
        bz_givePlayerFlag(cappingPlayer, teamToFlagType(basePoint), true);
      }

// Data
// ---
// (int)          playerID - ID of the player who was added to the world.
// (bz_eTeamType) team - The team the player is a member of.
// (bz_PlayerUpdateState) state - The state record for the spawning player
// (double)       eventTime - Time local server time for the event.
        
    }break;


    default:{ 
    }break;
  }
}

