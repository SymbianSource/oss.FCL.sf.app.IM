/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class gives interface to play IM message alert
 *
*/


// INCLUDE FILES
#include    "CCAAudioManager.h"
#include    "chatdefinitions.h"
#include    "ChatDebugPrint.h"
#include    "MCAAudioManagerObserver.h"
#include    "MCANotifierInterface.h"
#include    "CANotifierFactory.h"
#include    "imnginternalcrkeys.h"
#include    "imprivatecrkeys.h"

#include    <MProfileEngine.h>
#include	<pathinfo.h>
#include    <mda/common/resource.h>
#include    <audiopreference.h>
#include    <bautils.h>

#include    <hwrmvibrasdkcrkeys.h>
#include    <coreapplicationuisdomainpskeys.h>
#include    <ProfileEngineSDKCRKeys.h>

#include    <MProfileTones.h>
#include    <TProfileToneSettings.h>
#include    <MProfile.h>

#include    <HWRMVibra.h>




#include <cimpspresenceconnectionuing.h>
#include <CIMPSSAPSettingsStore.h>
#include <CIMPSSAPSettings.h>
#include <CIMPSSAPSettingsList.h>
#include <CPEngNWSessionSlotID2.h>



// CONSTANTS
const TInt KMinVolumeLevel( 1 );
const TInt KMaxVolumeLevel( 10 );
const TInt KAscendingVolumeInterval( 3000000 ); // 3 seconds
const TInt KToneInterval( 1000000 ); // 1 second pause between tones

_LIT( KChatRngMimeType, "application/vnd.nokia.ringing-tone" );
_LIT( KBeepOnceSound, "beep.rng" );
_LIT( KProfileSilentTone, "No_Sound.wav" ); // see CCAToneSettingItem.cpp


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAAudioManager::CCAAudioManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAAudioManager::CCAAudioManager( CIMPSPresenceConnectionUi& aConnUi,
                                  TBool aIMLauncherInvoked )
        :   iAudioPlayerStatus( EAudioPlayerNotCreated ),
        iTonePlayerStatus( EAudioPlayerNotCreated ),
        iIsPlayingDefaultIMSound( EFalse ),
        iIMLauncherInvoked( aIMLauncherInvoked ),
        iConnUI( aConnUi )
    {
    }

// ---------------------------------------------------------
// CCAAudioManager::ConstructL
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCAAudioManager::ConstructL()
    {
    CHAT_DP_FUNC_ENTER( "ConstructL" );
    iProfileApi = CreateProfileEngineL();

    iSapSettings = CIMPSSAPSettingsStore::NewL();

    User::LeaveIfError( iFs.Connect() );

    iCenRep = CRepository::NewL( KCRUidIM );

    CHAT_DP_TXT( "CMdaAudioPlayerUtility::NewL()" );

    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this );
    iAudioPlayerStatus = EAudioPlayerReady;

    CHAT_DP_TXT( "CMdaAudioToneUtility::NewL()" );
    iTonePlayer = CMdaAudioToneUtility::NewL( *this );
    iTonePlayerStatus = EAudioPlayerReady;

    iProfilesRepository = CRepository::NewL( KCRUidProfileEngine );
    iVibraRepository = CRepository::NewL( KCRUidVibraCtrl );

    // Get initial settings from active profile
    iRingingVolumeNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                              *iProfilesRepository,
                                                              CCenRepNotifyHandler::EIntKey,
                                                              KProEngActiveRingingVolume );
    iRingingVolumeNotifyHandler->StartListeningL();


    iRingingTypeNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                            *iProfilesRepository,
                                                            CCenRepNotifyHandler::EIntKey,
                                                            KProEngActiveRingingType );
    iRingingTypeNotifyHandler->StartListeningL();


    iVibraNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iVibraRepository,
                                                      CCenRepNotifyHandler::EIntKey,
                                                      KVibraCtrlProfileVibraEnabled );
    iVibraNotifyHandler->StartListeningL();

    User::LeaveIfError( iProfilesRepository->Get( KProEngActiveRingingVolume,
                                                  iRingingVolume ) );

    User::LeaveIfError( iVibraRepository->Get( KVibraCtrlProfileVibraEnabled,
                                               iVibra ) );

    User::LeaveIfError( iApaSession.Connect() );

    // register for changes in message Tone quitting
    iNotifierAPI = CANotifierFactory::CreateNotifierL( this,
                                                       KPSUidCoreApplicationUIs );
    iNotifierAPI->ListenKeyChanges( MCANotifierInterface::EMSGToneQuitKey );

    iToneFileName = HBufC::NewL( KMaxPath );

    // Get path to beep once ringing tone, first check simple sound folder.
    // Beep file might be in different folder
    // in different products.
    TFileName beepFile = PathInfo::RomRootPath();
    beepFile.Append( PathInfo::SimpleSoundsPath() );
    beepFile.Append( KBeepOnceSound );
    RFile file;
    TInt openErr = file.Open( iFs, beepFile, EFileRead );
    file.Close();

    if ( openErr == KErrNotFound || openErr == KErrPathNotFound )
        {
        // Check digital sounds folder
        beepFile.Zero();
        beepFile.Append( PathInfo::RomRootPath() );
        beepFile.Append( PathInfo::DigitalSoundsPath() );
        beepFile.Append( KBeepOnceSound );
        openErr = file.Open( iFs, beepFile, EFileRead );
        file.Close();
        }

    CHAT_DP( D_PLAIN_LIT( "CCAAudioManager::ConstructL() openErr = %d" ), openErr );

    if ( openErr == KErrNone )
        {
        iBeepRingingTone = beepFile.AllocL();
        TPtrC ptr = iBeepRingingTone->Des();
        CHAT_DP( D_PLAIN_LIT( "CCAAudioManager::ConstructL() iBeepRingingTone = %S" ), &ptr );
        }
    else
        {
        HandleError( openErr );
        }

    // get active ringing type, change events come to us later...
    iActiveProfile = iProfileApi->ActiveProfileL();
    const MProfileTones& profileTones = iActiveProfile->ProfileTones();
    const TProfileToneSettings& profileSettings = profileTones.ToneSettings();
    iRingingType = profileSettings.iRingingType;
    iActiveProfile->Release();
    iActiveProfile = NULL; // prevent further usage
    // after this, active ringing type changes are notified via the cenrep
    // notify handler callback through HandleNotifyInt

    // TRAP to catch leaves, leaves with KErrNotSupported if vibra
    // is not supported
    TRAPD( err, iVibrator = CHWRMVibra::NewL() );
    if ( err == KErrNotSupported )
        {
        // Make sure it's NULL
        iVibrator = NULL;
        }
    else
        {
        // Leave with other errors e.g. KErrNoMemory
        User::LeaveIfError( err );
        }

    CHAT_DP_FUNC_DONE( "ConstructL" );
    }

// Symbian OS default constructor can leave.
EXPORT_C CCAAudioManager* CCAAudioManager::NewL( CIMPSPresenceConnectionUi& aConnUi,
                                                 TBool aIMLauncherInvoked /*= EFalse*/ )
    {
    CCAAudioManager* self = new( ELeave ) CCAAudioManager( aConnUi, aIMLauncherInvoked );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CCAAudioManager::~CCAAudioManager()
    {
    CHAT_DP_FUNC_ENTER( "~CCAAudioManager" );
    Stop(); // Stops possible playback

    if ( iActiveProfile )
        {
        iActiveProfile->Release();
        iActiveProfile = NULL; // prevent further usage
        }

    if ( iProfileApi )
        {
        iProfileApi->Release();
        }

    if ( iVibraNotifyHandler )
        {
        iVibraNotifyHandler->StopListening();
        delete iVibraNotifyHandler;
        }

    if ( iRingingTypeNotifyHandler )
        {
        iRingingTypeNotifyHandler->StopListening();
        delete iRingingTypeNotifyHandler;
        }


    if ( iRingingVolumeNotifyHandler )
        {
        iRingingVolumeNotifyHandler->StopListening();
        delete iRingingVolumeNotifyHandler;
        }

    delete iProfilesRepository;
    delete iVibraRepository;

    iApaSession.Close();
    delete iNotifierAPI;

    CHAT_DP_TXT( "delete CMdaAudioPlayerUtility" );
    iAudioPlayer->Close();
    delete iAudioPlayer;

    CHAT_DP_TXT( "delete CMdaAudioToneUtility" );
    delete iTonePlayer;

    iObservers.Close();

    delete iToneFileName;

    delete iVibrator;

    delete iBeepRingingTone;

    delete iCenRep;

    delete iSapSettings;

    iFs.Close();

    CHAT_DP_FUNC_DONE( "~CCAAudioManager" );
    }

// ---------------------------------------------------------
// CCAAudioManager::Play
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::PlayL()
    {
    CHAT_DP_FUNC_ENTER( "Play" );
    Stop();
    TRAPD( err, DoPlayL() );
    if ( err )
        {
        NotifyObservers();
        HandleError( err );
        }
    CHAT_DP_FUNC_DONE( "Play" );
    }

// ---------------------------------------------------------
// CCAAudioManager::DoPlayL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::DoPlayL()
    {
    CHAT_DP_FUNC_ENTER( "DoPlayL" );

    TBuf<KMaxPath> toneFileName;

    // check if we're silent or not
    iActiveProfile = iProfileApi->ActiveProfileL();
    TBool isSilent = iActiveProfile->IsSilent();
    iActiveProfile->Release();
    iActiveProfile = NULL; // prevent further usage

    // vibrate, if it's allowed
    CHWRMVibra::TVibraModeState vibraModeState = iVibrator->VibraSettings();
    CHWRMVibra::TVibraStatus vibraStatus = iVibrator->VibraStatus();

    const TInt KIMVibraDuration = 1000; // in milliseconds
    if ( vibraModeState == CHWRMVibra::EVibraModeON &&
         vibraStatus == CHWRMVibra::EVibraStatusStopped )
        {
        iVibrator->StartVibraL( KIMVibraDuration );
        }

    // sound ordering:
    //  silent: get out.
    //  beep once: beep, and if ok, exit.
    //  if beep failed, or not active: try to play sound
    //  for this server.
    //  if it failed, play default sound.

    if ( isSilent )
        {
        // Silent profile, get out.
        CHAT_DP_TXT( "DoPlayL - Silent profile, notifying observers" );
        NotifyObservers();
        CHAT_DP_TXT( "DoPlayL - Returning" );
        return;
        }

    TInt err = KErrNone;

    if ( iRingingType == EProfileRingingTypeBeepOnce && iBeepRingingTone )
        {
        // Play beep.rng
        TRAP( err, ProcessFileL( *iBeepRingingTone ) );
        }

    if ( err != KErrNone || iRingingType != EProfileRingingTypeBeepOnce )
        {
        err = KErrNone;

        // Beep once not active, or beep once failed.
        // Try to play the sound set for this server
        TBuf<KMaxPath> toneFileName;
        GetToneFileNameL( toneFileName );
        TParsePtr parsedPtr( toneFileName );

        CHAT_DP( D_CHAT_LIT( " file = [%S] " ), &toneFileName );

        // If alert tone OFF, don't play anything
        if ( parsedPtr.NameAndExt().Compare( KProfileSilentTone ) != 0 )
            {
            // Not OFF
            TRAP( err, ProcessFileL( toneFileName ) );
            }

        if ( err )
            {
            // Trying to play default sound, because current sound failed
            err = KErrNone;
            ProcessDefaultAlertSound();
            // The above routine shows errors if it fails
            }
        }

    // Not ringing once, but left
    User::LeaveIfError( err );

    CHAT_DP_FUNC_DONE( "DoPlayL" );
    }

// ---------------------------------------------------------
// CCAAudioManager::Stop
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::Stop()
    {
    CHAT_DP_FUNC_ENTER( "Stop" );

    if ( iAudioPlayerStatus == EAudioPlayerPlaying )
        {
        CHAT_DP_TXT(
            "Audio player is playing. CMdaAudioPlayerUtility::Stop()" );
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
        iAudioPlayerStatus = EAudioPlayerReady;
        }

    if ( iTonePlayerStatus == EAudioPlayerPlaying )
        {
        CHAT_DP_TXT(
            "Tone player is playing. CMdaAudioToneUtility::CancelPlay()" );
        iTonePlayer->CancelPlay();
        iTonePlayerStatus = EAudioPlayerReady;
        }

    CHAT_DP_FUNC_DONE( "Stop" );
    }

// ---------------------------------------------------------
// CCAAudioManager::SetVolume
// Set the volume at which the sound is played
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::SetVolume( TInt aVolume )
    {
    iRingingVolumeNotifyHandler->StopListening();
    iRingingVolume = aVolume;
    }

// ---------------------------------------------------------
// CCAAudioManager::SetRingingType
// Sets the ringing type how the sound is played
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::SetRingingType( TProfileRingingType aRingingType )
    {
    iRingingTypeNotifyHandler->StopListening();
    iRingingType = aRingingType;
    }

// ---------------------------------------------------------
// CCAAudioManager::SetVibra
// Sets the vibrating alert on or off
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::SetVibra( TBool aVibra )
    {
    iVibraNotifyHandler->StopListening();
    iVibra = aVibra;
    }

// ---------------------------------------------------------
// CCAAudioManager::AddObserverL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::AddObserverL( const MCAAudioManagerObserver* aObserver )
    {
    __ASSERT_DEBUG( aObserver, User::Panic( KPanicText, KErrArgument ) );

    // make sure that aObserver does not get appended in release mode also.
    if ( aObserver )
        {
        User::LeaveIfError( iObservers.Append( aObserver ) );
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::RemoveObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCAAudioManager::RemoveObserver( const MCAAudioManagerObserver* aObserver )
    {
    TInt index( iObservers.Find( aObserver ) );

    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::ProcessDesSequenceL
// If des sequence is defined, prepare the audioplayer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::ProcessDesSequence( const TDesC8& aSequence )
    {
    CHAT_DP_FUNC_ENTER( "ProcessDesSequence" );
    Stop(); // Stops possible playback

    if ( iTonePlayerStatus == EAudioPlayerReady )
        {
        CHAT_DP_TXT( "CMdaAudioToneUtility::PrepareToPlayDesSequence" );
        iTonePlayerStatus = EAudioPlayerInitializing;
        iTonePlayer->PrepareToPlayDesSequence( aSequence );
        }

    CHAT_DP_FUNC_DONE( "ProcessDesSequence" );
    }

// ---------------------------------------------------------
// CCAAudioManager::ProcessDefaultAlertSound
// If des sequence is defined, prepare the audioplayer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::ProcessDefaultAlertSound( )
    {
    iIsPlayingDefaultIMSound = ETrue;

    TRAPD( err, ProcessFileL( KNullDesC ) );

    if ( err )
        {
        HandleError( err );
        NotifyObservers();
        iIsPlayingDefaultIMSound = EFalse;
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::ProcessFileL
// If filename is defined, prepare the audioplayer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::ProcessFileL( const TDesC& aFileName )
    {
    CHAT_DP_FUNC_ENTER( "ProcessFileL" );

    TFileName *fileName = new ( ELeave ) TFileName();
    CleanupStack::PushL( fileName );

    // if filename is empty, use default sound
    if ( aFileName.Length() == 0 )
        {
        DefaultSoundFile( *fileName );
        }
    else
        {
        *fileName = aFileName;
        }

    TPtr filePtr( NULL, 0 );
    if ( !BaflUtils::FileExists( iFs, *fileName ) )
        {
        CHAT_DP( D_CHAT_LIT( "File does not exist ( %S )" ), &( *fileName ) );
        User::Leave( KErrNotFound );
        }

    Stop(); // Stops possible playback

    TUid dummyUid( KNullUid );
    TDataType dataType;
    User::LeaveIfError( iApaSession.AppForDocument( *fileName, dummyUid,
                                                    dataType ) );

    if ( dataType.Des().CompareF( KChatRngMimeType ) == 0 )
        {
        // making sure that another initializing is not already ongoing
        if ( iTonePlayerStatus == EAudioPlayerReady )
            {
            CHAT_DP( D_CHAT_LIT(
                         "CMdaAudioToneUtility::PrepareToPlayFileSequence( %S )" ),
                     &( *fileName ) );
            iTonePlayerStatus = EAudioPlayerInitializing;
            iTonePlayer->PrepareToPlayFileSequence( *fileName );
            }
        }
    else
        {
        // making sure that another initializing is not already ongoing
        if ( iAudioPlayerStatus == EAudioPlayerReady )
            {
            CHAT_DP( D_CHAT_LIT(
                         "FileList - CMdaAudioPlayerUtility::OpenFileL( %S )" ),
                     &( *fileName ) );
            iAudioPlayerStatus = EAudioPlayerInitializing;
            TRAPD( err, iAudioPlayer->OpenFileL( *fileName ) );
            CHAT_DP( D_CHAT_LIT(
                         "FileList - CMdaAudioPlayerUtility::OpenFileL err = %d" ),
                     err );
            if ( err )
                {
                iAudioPlayerStatus = EAudioPlayerReady;
                User::Leave( err );
                }
            }
        }

    CleanupStack::PopAndDestroy( fileName );
    CHAT_DP_FUNC_DONE( "ProcessFileL" );
    }

// ---------------------------------------------------------
// CCAAudioManager::DoSetRingingType
// When audioplayer is ready, set its ringing type
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::DoSetRingingType( TInt aRingingType )
    {
    if ( iAudioPlayerStatus == EAudioPlayerInitialized )
        {
        switch ( aRingingType )
            {
                // Fall through
            case EProfileRingingTypeRinging:
            case EProfileRingingTypeSilent:
                {
                iAudioPlayer->SetRepeats( KMdaRepeatForever,
                                          TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            case EProfileRingingTypeAscending:
                {
                iAudioPlayer->SetRepeats( KMdaRepeatForever,
                                          TTimeIntervalMicroSeconds( KToneInterval ) );
                iAudioPlayer->SetVolumeRamp(
                    TTimeIntervalMicroSeconds( KAscendingVolumeInterval ) );
                break;
                }
            case EProfileRingingTypeRingingOnce:
                {
                iAudioPlayer->SetRepeats( 0,
                                          TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            case EProfileRingingTypeBeepOnce:
                {
                iAudioPlayer->SetRepeats( 0,
                                          TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( iTonePlayerStatus == EAudioPlayerInitialized )
        {
        switch ( aRingingType )
            {
                // Fall through
            case EProfileRingingTypeRinging:
            case EProfileRingingTypeSilent:
                {
                iTonePlayer->SetRepeats( KMdaRepeatForever,
                                         TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            case EProfileRingingTypeAscending:
                {
                iTonePlayer->SetRepeats( KMdaRepeatForever,
                                         TTimeIntervalMicroSeconds( KToneInterval ) );
                iTonePlayer->SetVolumeRamp(
                    TTimeIntervalMicroSeconds( KAscendingVolumeInterval ) );
                break;
                }
            case EProfileRingingTypeRingingOnce:
                {
                iTonePlayer->SetRepeats( 0,
                                         TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            case EProfileRingingTypeBeepOnce:
                {
                iTonePlayer->SetRepeats( 0,
                                         TTimeIntervalMicroSeconds( KToneInterval ) );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::ConvertVolume
// Convert volume to the scale used by hardware.
// E.g. Profiles lets user select volume 1-10, but hw's scale is normally 1-9.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAAudioManager::ConvertVolume( TInt aVolume )
    {
    TInt result( 0 );
    // if ringing type is "silent" then volume
    // is zero
    if ( iRingingType != EProfileRingingTypeSilent )
        {
        if ( iAudioPlayerStatus == EAudioPlayerInitialized ||
             iAudioPlayerStatus == EAudioPlayerPlaying )
            {
            result = iAudioPlayer->MaxVolume() * aVolume / KMaxVolumeLevel;
            if ( aVolume == KMinVolumeLevel && result == 0 )
                {
                result = 1;
                }
            }
        else if ( iTonePlayerStatus == EAudioPlayerInitialized ||
                  iTonePlayerStatus == EAudioPlayerPlaying )
            {
            result = iTonePlayer->MaxVolume() * aVolume / KMaxVolumeLevel;
            if ( aVolume == KMinVolumeLevel && result == 0 )
                {
                result = 1;
                }
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CCAAudioManager::NotifyObservers
// Notifies all observers that playing is completed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::NotifyObservers()
    {
    CHAT_DP_FUNC_ENTER( "NotifyObservers" );

    TInt observerCount( iObservers.Count() );

    for ( TInt i( 0 ); i < observerCount; ++i )
        {
        ( *iObservers[i] ).PlayCompleted();
        }

    CHAT_DP_FUNC_DONE( "NotifyObservers" );
    }

// ---------------------------------------------------------
// CCAAudioManager::HandleError
// Notifies all observers that playing is completed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::HandleError( TInt aError )
    {
    if ( ( aError == KErrNoMemory ) ||
         ( aError == KErrDiskFull ) )
        {
        CActiveScheduler::Current()->Error( aError );
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::GetToneFileNameL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::GetToneFileNameL( TDes& aToneFileName )
    {

    CIMPSSAPSettings* sap = NULL;
    sap = StoredSAPLC();

    // populate the list of sapsettings
    CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
    iSapSettings->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

    // and find our sap.. because logged in SAP has UID of zero, we must
    // find the correct UID manually from list
    TInt index( KErrNotFound );
    sapList->FindNameL( sap->SAPName(), index );

    if ( index != KErrNotFound )
        {
        // found it, update the correct sap
        CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
        iSapSettings->GetSAPL( sapList->UidForIndex( index ), storedSap );

        TPtrC audioPath;
        storedSap->GetOpaqueDesC16( KIMAlertTonePath(), audioPath );
        aToneFileName.Copy( audioPath );
        CleanupStack::PopAndDestroy( storedSap );
        }
    else
        {
        aToneFileName.Copy( KNullDesC() );
        }

    CleanupStack::PopAndDestroy( 2, sap );

    }

// ---------------------------------------------------------
// CCAAudioManager::DefaultSoundFile
// Attention: This same function exists also in CCAToneSettingItem,
// because IMUtils cannot be used here. So if changing here, change also
// there.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::DefaultSoundFile( TDes& aSoundFileName )
    {
    CHAT_DP_FUNC_ENTER( "DefaultSoundFile" );

    TInt err = iCenRep->Get( KIMAlertTone, aSoundFileName );

    if ( err )
        {
        CActiveScheduler::Current()->Error( err );
        }

    CHAT_DP_FUNC_DONE( "DefaultSoundFile" );
    }

// ---------------------------------------------------------
// CCAAudioManager::MapcInitComplete
// This method is called when the audioplayer initialisation is ready.
// If file format is supported, start playing the sound file.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::MapcInitComplete(
    TInt aError, const TTimeIntervalMicroSeconds& /* aDuration */ )
    {
    CHAT_DP( D_CHAT_LIT(
                 "MMdaAudioPlayerCallback::MapcInitComplete() callback, error == %d" ),
             aError );

    if ( !aError && iAudioPlayerStatus == EAudioPlayerInitializing )
        {
        iAudioPlayerStatus = EAudioPlayerInitialized;
        // currently IM messages supports only this ringing type
        // ringing once
        DoSetRingingType( EProfileRingingTypeRingingOnce );
        CHAT_DP( D_CHAT_LIT( "CMdaAudioPlayerUtility::SetVolume( %d )" ),
                 iRingingVolume );

        iAudioPlayer->SetVolume( ConvertVolume( iRingingVolume ) );

        CHAT_DP( D_CHAT_LIT( "CMdaAudioPlayerUtility::SetPriority( %d, %d )" ),
                 KAudioPriorityRecvMsg,
                 TMdaPriorityPreference( KAudioPrefNewSpecialMessage ) );

        iAudioPlayer->SetPriority( KAudioPriorityRecvMsg,
                                   TMdaPriorityPreference( KAudioPrefNewSpecialMessage ) );

        CHAT_DP_TXT( "CMdaAudioPlayerUtility::Play()" );
        iAudioPlayer->Play();
        iAudioPlayerStatus = EAudioPlayerPlaying;
        iNotifierAPI->SetInt( MCANotifierInterface::EMSGToneQuitKey,
                              ECoreAppUIsTonePlaying );
        }
    else
        {
        Stop();
        iAudioPlayerStatus = EAudioPlayerReady;
        if ( ( ( aError == KErrNotSupported ) || ( aError == KErrCorrupt ) )
             && !iIsPlayingDefaultIMSound )
            {
            ProcessDefaultAlertSound();
            }
        else
            {
            HandleError( aError );
            NotifyObservers();
            iIsPlayingDefaultIMSound = EFalse;
            }
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::MapcPlayComplete
// This method is called when the audioplayer has finished playing a sound
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::MapcPlayComplete( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT(
                 "MMdaAudioPlayerCallback::MapcPlayComplete() callback, error == %d" ),
             aError );

    iNotifierAPI->SetInt( MCANotifierInterface::EMSGToneQuitKey, 0 );
    iAudioPlayer->Close();
    iAudioPlayerStatus = EAudioPlayerReady;

    if ( ( ( aError == KErrNotSupported ) || ( aError == KErrCorrupt ) ) &&
         !iIsPlayingDefaultIMSound )
        {
        ProcessDefaultAlertSound();
        }
    else
        {
        HandleError( aError );
        iIsPlayingDefaultIMSound = EFalse;
        NotifyObservers();
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::HandleNotifyL
// This method receives notifications from the shared data server
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::HandleKeyChange( TUpdatedKey& aUpdatedValues )
    {
    CHAT_DP_FUNC_ENTER( "HandleKeyChange" );

    // If we have an event which changes the key value to
    // ECoreAppUIsStopTonePlaying -> stop playing.
    if ( aUpdatedValues.iValue == ECoreAppUIsStopTonePlaying )
        {
        // Message tone should be stopped
        CHAT_DP_TXT( "**stop playing!**" );
        Stop();
        NotifyObservers();
        }

    CHAT_DP_FUNC_DONE( "HandleKeyChange" );
    }

void CCAAudioManager::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    CHAT_DP_FUNC_ENTER( "HandleNotifyL" );
    CHAT_DP( D_CHAT_LIT( "aKey == %d" ), aId );
    CHAT_DP( D_CHAT_LIT( "aValue == %d" ), aNewValue );


    if ( aId == KProEngActiveRingingVolume  )  // Ringing volume has changed
        {
        iRingingVolume = aNewValue;
        }
    else if ( aId == KProEngActiveRingingType )  // Ringing type has changed
        {
        iRingingType = aNewValue;
        }

    if ( aId == KVibraCtrlProfileVibraEnabled )  // Vibra setting has changed
        {
        iVibra = aNewValue;
        }

    CHAT_DP_FUNC_DONE( "HandleNotifyL" );
    }

// ---------------------------------------------------------
// CCAAudioManager::MatoPrepareComplete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::MatoPrepareComplete( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT(
                 "MMdaAudioToneObserver::MatoPrepareComplete() callback, error == %d" ),
             aError );

    if ( !aError && iTonePlayerStatus == EAudioPlayerInitializing )
        {
        iTonePlayerStatus = EAudioPlayerInitialized;
        // currently IM messages supports only this ringing type
        // ringing once
        DoSetRingingType( EProfileRingingTypeRingingOnce );
        CHAT_DP( D_CHAT_LIT( "CMdaAudioToneUtility::SetVolume( %d )" ),
                 ConvertVolume( iRingingVolume ) );
        iTonePlayer->SetVolume( ConvertVolume( iRingingVolume ) );


        CHAT_DP( D_CHAT_LIT( "CMdaAudioToneUtility::SetPriority( %d, %d )" ),
                 KAudioPriorityRecvMsg,
                 TMdaPriorityPreference( KAudioPrefNewSpecialMessage ) );

        iTonePlayer->SetPriority( KAudioPriorityRecvMsg,
                                  TMdaPriorityPreference( KAudioPrefNewSpecialMessage ) );

        CHAT_DP_TXT( "CMdaAudioToneUtility::Play()" );
        iTonePlayer->Play();
        iTonePlayerStatus = EAudioPlayerPlaying;
        iNotifierAPI->SetInt( MCANotifierInterface::EMSGToneQuitKey,
                              ECoreAppUIsTonePlaying );
        }
    else
        {
        Stop();
        iTonePlayerStatus = EAudioPlayerReady;
        if ( ( ( aError == KErrNotSupported ) || ( aError == KErrCorrupt ) )
             && !iIsPlayingDefaultIMSound )
            {
            ProcessDefaultAlertSound();
            }
        else
            {
            HandleError( aError );
            NotifyObservers();
            iIsPlayingDefaultIMSound = EFalse;
            }
        }
    }

// ---------------------------------------------------------
// CCAAudioManager::MatoPlayComplete
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAAudioManager::MatoPlayComplete( TInt aError )
    {
    CHAT_DP( D_CHAT_LIT(
                 "MMdaAudioToneObserver::MatoPlayComplete() callback, error == %d" ),
             aError );

    iNotifierAPI->SetInt( MCANotifierInterface::EMSGToneQuitKey, 0 );
    iTonePlayerStatus = EAudioPlayerReady;

    if ( ( ( aError == KErrNotSupported ) || ( aError == KErrCorrupt ) ) &&
         !iIsPlayingDefaultIMSound )
        {
        ProcessDefaultAlertSound();
        }
    else
        {
        HandleError( aError );
        iIsPlayingDefaultIMSound = EFalse;
        NotifyObservers();
        }
    }



// ---------------------------------------------------------
// CCAAudioManager::StoredSAPLC
// (other items were commented in a header).
// ---------------------------------------------------------
//
CIMPSSAPSettings* CCAAudioManager::StoredSAPLC()
    {
    CIMPSSAPSettings* sap = CIMPSSAPSettings::NewLC();

    TBool isLoggedIn( EFalse );

    TInt err;
    TRAP( err, isLoggedIn = iConnUI.LoggedInL( EIMPSConnClientIM ) );

    if ( isLoggedIn )
        {
        CPEngNWSessionSlotID2* sessionSlotId =
            iConnUI.GetActiveNWSessionSlotIDL( EIMPSConnClientIM );
        CleanupStack::PushL( sessionSlotId );
        iConnUI.GetLoggedInSapL( *sessionSlotId, *sap );

        // populate the list of sapsettings
        CIMPSSAPSettingsList* sapList = CIMPSSAPSettingsList::NewLC();
        iSapSettings->PopulateSAPSettingsListL( *sapList, EIMPSIMAccessGroup );

        // and find our sap.. because logged in SAP has UID of zero, we must
        // find the correct UID manually from list
        TInt index( KErrNotFound );
        TPtrC name( sap->SAPName() );
        CHAT_DP( D_CHAT_LIT( "logged to to %S, trying to find sap" ), &name );
        sapList->FindNameL( name, index );

        if ( index != KErrNotFound )
            {
            CHAT_DP( D_CHAT_LIT( "found it in index %d" ), index );
            // found it, update the correct sap
            CIMPSSAPSettings* storedSap = CIMPSSAPSettings::NewLC();
            iSapSettings->GetSAPL( sapList->UidForIndex( index ), storedSap );
            CleanupStack::Pop( storedSap );
            CleanupStack::PopAndDestroy( 3, sap ); // sapList, sessionslot, sap
            CleanupStack::PushL( storedSap );
            sap = storedSap;
            }
        else
            {
            CHAT_DP( D_CHAT_LIT( "not found" ) );
            CleanupStack::PopAndDestroy( 3, sap ); // sapList, sessionSlotId, sap
            return NULL;
            }
        }
    else if ( iSapSettings )
        {
        iSapSettings->GetDefaultL( sap, EIMPSIMAccessGroup );
        }

    return sap;
    }

//  End of File
