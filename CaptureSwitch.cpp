// CaptureSwitch.cpp

#include "bzfsAPI.h"

class CaptureSwitch : public bz_Plugin
{
public:
  const char* Name(){return "CaptureSwitch";}
  void Init ( const char* /*config*/ );
  void Event(bz_EventData *eventData );
  void Cleanup ( void );
  int cappingPlayer = -1;
  bz_eTeamType cappingTeam = eNoTeam;
};

BZ_PLUGIN(CaptureSwitch)

void CaptureSwitch::Init (const char* commandLine) {
  Register(bz_eAllowCTFCaptureEvent);
  Register(bz_eCaptureEvent);
  Register(bz_eTickEvent);
  MaxWaitTime = 0.3f;
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
        if (bz_getPlayerFlagID(allowCapData->playerCapping) != -1) {
            bz_resetFlag(bz_getPlayerFlagID(allowCapData->playerCapping));
            cappingPlayer = allowCapData->playerCapping;
            cappingTeam = bz_getPlayerTeam(allowCapData->playerCapping);
            // This shouldn't be so tedious to do.
        } 
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
            cappingPlayer = -1;
            cappingTeam = eNoTeam;
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
      if (cappingPlayer != -1) {
        if (bz_triggerFlagCapture(cappingPlayer, cappingTeam, eRedTeam)) {
            puts("made a cap");
        }
      }
      //puts("");
    }break;

    default:{ 
    }break;
  }
}

