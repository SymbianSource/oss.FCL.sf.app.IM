/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Test utilities for WV engine
*
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <CoreApplicationUIsSDKCRKeys.h>
#include    "impsservercommon.h"
#include    "impsutils.h"
#include    "impserrors.h"
#include    "impsservices.h"
#include    "impssrvutils.h"
#include    "impskey.h"
#include    "impssdatautils.h"
#include    "impsfields.h"
#include    "wvengineinternalcrkeys.h"

// MACROS
#ifdef _DEBUG
#define _IMPS_LOGGING_
#endif

// CONSTANTS
#ifdef _DEBUG
_LIT8( KImpsIniPollTime, "PollTime" );
_LIT8( KImpsIniXmlLog, "XmlLog" );
_LIT8( KImpsIniXmlPollingLog, "XmlPollingLog" );
_LIT8( KImpsIniNoWAPSMSCIR, "NoWAPSMSCIR" );
_LIT8( KImpsIniNoWAPUDPCIR, "NoWAPUDPCIR" );
_LIT8( KImpsIniNoTCPSCIR, "NoTCPSCIR" );
_LIT8( KImpsIniNoUDPSCIR, "NoUDPSCIR" );
_LIT8( KImpsIniKeepAlive, "KeepAlive" );
_LIT8( KImpsIniCIRModePollTime, "CIRModePollTime" );
_LIT8( KImpsIniMultiTrans, "MultiTrans" );
_LIT8( KImpsIniACExpiry, "ACExpiry" );
_LIT8( KImpsIniGRExpiry, "GRExpiry" );
_LIT8( KImpsIniPRExpiry, "PRExpiry" );
_LIT8( KImpsIniFUExpiry, "FUExpiry" );
_LIT8( KImpsIniIMExpiry, "IMExpiry" );
_LIT8( KImpsIniPDPExpiry, "PDPExpiry" );
_LIT8( KImpsIni4WayLogin, "4WayLogin" );
_LIT8( KImpsIniMaxMessageSize, "MaxMessageSize" );
#endif


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TImpsSrvUtils::InitializeServices
// ---------------------------------------------------------
//
void TImpsSrvUtils::InitializeServices(
    TImpsServices& aServices, TBool aReactive )
    {

    // clear the service tree
    Mem::FillZ( &aServices, sizeof( TImpsServices ) );

    // IM Send NewMessage and receive are requested plus IMAuthFunc
    TImpsFeature* feat = aServices.Feature( EIMFeat );
    feat->SetSupported( ETrue );
    feat->SetFunction( EAllFuncs , ETrue );
    feat->SetCapability( KImpsServiceNEWM | KImpsServiceGLBLU
                         | KImpsServiceBLENT , ETrue );

    // All group features are requested
    feat = aServices.Feature( EGroupFeat );
    feat->SetSupported( ETrue );
    feat->SetFunction( EAllFuncs, ETrue );
    feat->SetCapability( 0xFFFFFFFF , ETrue );  //lint !e569

    // Reactive Presence is optional, if it is supported
    // then the entire Presence is supported.
    // WV 1.2 contains a new element GETAUT but there is no
    // need to handle that separately, since the entire Presence
    // is negotiated then.
    feat = aServices.Feature( EPresenceFeat );
    feat->SetSupported( ETrue );
    if ( aReactive )
        {
        feat->SetFunction( EAllFuncs, ETrue );
        feat->SetCapability( 0xFFFFFFFF , ETrue );  //lint !e569
        }
    else
        {
        // Part of presence features are requested
        // PresenceAuth functions has limited support.
        feat->SetFunction( EPresenceAuthFunc, ETrue );
        feat->SetCapability(
            KImpsServiceGETWL, ETrue );
        feat->SetFunction( EContListFunc, ETrue );
        feat->SetCapability( KImpsServiceGCLI |
                             KImpsServiceCCLI | KImpsServiceDCLI | KImpsServiceMCLS, ETrue );
        feat->SetFunction( EAttrListFunc, ETrue );
        feat->SetCapability(
            KImpsServiceCALI | KImpsServiceDALI | KImpsServiceGALS , ETrue );
        feat->SetFunction( EPresenceDeliverFunc, ETrue );
        feat->SetCapability( KImpsServiceGETPR | KImpsServiceUPDPR, ETrue );
        }

    // Part of fundamental features are requested:
    //   SearchFunc and InviteFunc supported.
    //   ServiceFunc not supported.
    feat = aServices.Feature( EFundamentalFeat );
    feat->SetSupported( ETrue );
    feat->SetFunction( EInviteFunc, ETrue );
    feat->SetFunction( ESearchFunc , ETrue );
    feat->SetCapability( KImpsServiceINVIT | KImpsServiceCAINV |
                         KImpsServiceSRCH | KImpsServiceSTSRC, ETrue );
    }


// -----------------------------------------------------------------------------
// TImpsSrvUtils::DiscardServicesL
// ---------------------------------------------------------------------------
void TImpsSrvUtils::DiscardServicesL(
    MImpsDataAccessor* aData,
    TImpsServices* aServices  )
    {

    CImpsKey* myKey = CImpsKey::NewLC();   // <<< myKey
    TImpsSDataUtils::AddValuesFromArrayL(
        myKey,
        KClientServiceResElements,
        sizeof( KClientServiceResElements ) /
        sizeof( KClientServiceResElements[0] ) );

    // notice: ignore Client id this time

    // Discareded Functions
    myKey->AddL( CREATEKEY( EImpsKeyFunctions, 0 ) );
    if ( !aData->CheckBranchExistenceL( myKey ) )
        {
        // Nothing is denied.
        CleanupStack::PopAndDestroy( 1 );
        return;
        }

    myKey->AddL( CREATEKEY( EImpsKeyWVCSPFeat, 0 ) );
    TInt enumi( 0 );
    TInt index( 0 );
    TImpsKeyType kType;

    if ( aData->CheckBranchExistenceL( myKey ) )
        {
        // Check if empty WVCSPFeat. That means deny all.
        // Decoder may have inserted the element with empty data although
        // it has a child. Therefore RestoreEmptyL cannot be used here.
        if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
            {
            // All is denied
            TImpsFeature* imF = aServices->Feature( EIMFeat );
            imF->SetSupported( EFalse );
            imF->SetFunction( EAllFuncs, EFalse );
            imF->SetCapability( 0xFFFFFFFF , EFalse );

            TImpsFeature* prF = aServices->Feature( EPresenceFeat );
            prF->SetSupported( EFalse );
            prF->SetFunction( EAllFuncs, EFalse );
            prF->SetCapability( 0xFFFFFFFF , EFalse );

            TImpsFeature* funF = aServices->Feature( EFundamentalFeat );
            funF->SetSupported( EFalse );
            funF->SetFunction( EAllFuncs, EFalse );
            funF->SetCapability( 0xFFFFFFFF , EFalse );

            TImpsFeature* grF = aServices->Feature( EGroupFeat );
            grF->SetSupported( EFalse );
            grF->SetFunction( EAllFuncs, EFalse );
            grF->SetCapability( 0xFFFFFFFF , EFalse );
            CleanupStack::PopAndDestroy( 1 );
            return;
            }

        // IM
        myKey->AddL( CREATEKEY( EImpsKeyIMFeat, 0 ) );
        if ( aData->CheckBranchExistenceL( myKey ) )
            {
            TImpsFeature* imF = aServices->Feature( EIMFeat );
            if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                {
                // Whole IM denied
                imF->SetSupported( EFalse );
                imF->SetFunction( EAllFuncs, EFalse );
                imF->SetCapability( 0xFFFFFFFF , EFalse );
                }
            else
                {

                // Let's check functions too, although server should not send these
                // because of we have not ordered them.
                myKey->AddL( CREATEKEY( EImpsKeyIMSendFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // EImpsKeyIMSendFunc not supported
                        imF->SetFunction( EIMSendFunc, EFalse );
                        }
                    // we are not interested in IMSendFunc details now.
                    }
                // IMReceiveFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyIMReceiveFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // EImpsKeyIMReceiveFunc not supported
                        imF->SetFunction( EIMReceiveFunc, EFalse );
                        imF->SetCapability( KImpsServiceNEWM , EFalse );
                        }
                    else
                        {
                        // Check if NEWM not supported
                        myKey->AddL( CREATEKEY( EImpsKeyNEWM , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            imF->SetCapability( KImpsServiceNEWM, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                // IMAuthFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyIMAuthFunc , 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // IMAuthFunc not supported
                        imF->SetFunction( EIMAuthFunc, EFalse );
                        imF->SetCapability( KImpsServiceGLBLU | KImpsServiceBLENT, EFalse );
                        }
                    else
                        {
                        // Check if GLBLU not supported
                        myKey->AddL( CREATEKEY( EImpsKeyGLBLU , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            imF->SetCapability( KImpsServiceGLBLU, EFalse );
                            }
                        // Check if BLENT not supported
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyBLENT , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            imF->SetCapability( KImpsServiceBLENT, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                myKey->PopL();
                }
            } // IM

        // Presence

        myKey->ReplaceLastL( CREATEKEY( EImpsKeyPresenceFeat, 0 ) );
        if ( aData->CheckBranchExistenceL( myKey ) )
            {
            TImpsFeature* prF = aServices->Feature( EPresenceFeat );
            if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                {
                // Presence totally denied
                prF->SetSupported( EFalse );
                prF->SetFunction( EAllFuncs, EFalse );
                prF->SetCapability( 0xFFFFFFFF , EFalse );
                }
            else
                {
                // Check first AttListFunc
                myKey->AddL( CREATEKEY( EImpsKeyAttListFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // AttListFunc not supported
                        prF->SetFunction( EAttrListFunc, EFalse );
                        prF->SetCapability( KImpsServiceCALI | KImpsServiceDALI |
                                            KImpsServiceGALS, EFalse );
                        }
                    else
                        {
                        // Check CALI
                        myKey->AddL( CREATEKEY( EImpsKeyCALI , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceCALI, EFalse );
                            }
                        // Check DALI
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyDALI , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceDALI, EFalse );
                            }
                        // Check GALS
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGALS , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceGALS, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                // PresenceDeliverFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyPresenceDeliverFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // PresenceDeliverFunc not supported
                        prF->SetFunction( EPresenceDeliverFunc, EFalse );
                        prF->SetCapability( KImpsServiceGETPR | KImpsServiceUPDPR, EFalse );
                        }
                    else
                        {
                        // Check GETPR
                        myKey->AddL( CREATEKEY( EImpsKeyGETPR , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceGETPR, EFalse );
                            }
                        // Check UPDPR
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyUPDPR , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceUPDPR, EFalse );
                            }
                        myKey->PopL();
                        }
                    }

                // PresenceAuthFunc
                // Get the WV version since WV 1.2 includes a brand new
                // element GETAUT.
                CImpsFields* f = aData->GetImpsFields();
                TImpsCspVersion ver = f->CspVersion();
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyPresenceAuthFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // PresenceAuthFunc not supported
                        prF->SetFunction( EPresenceAuthFunc, EFalse );
                        prF->SetCapability( KImpsServiceGETWL | KImpsServiceREACT |
                                            KImpsServiceCAAUT, EFalse );
                        }
                    else
                        {
                        // Check GETWL
                        myKey->AddL( CREATEKEY( EImpsKeyGETWL, 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceGETWL, EFalse );
                            }
                        // Check REACT
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyREACT, 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceREACT, EFalse );
                            }
                        // Check CAAUT
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyCAAUT, 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            prF->SetCapability( KImpsServiceCAAUT, EFalse );
                            }
                        // Check GETAUT, clear it always in WV 1.1
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGETAUT, 0 ) );
                        if ( ver == EImpsCspVersion11 ||
                             ( aData->RestoreEmptyL( myKey ) ) )
                            {
                            prF->SetCapability( KImpsServiceGETAUT, EFalse );
                            }
                        myKey->PopL();
                        }

                    // ContactListFunc
                    myKey->ReplaceLastL( CREATEKEY( EImpsKeyContListFunc, 0 ) );
                    if ( aData->CheckBranchExistenceL( myKey ) )
                        {
                        if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                            {
                            // ContactListFunc not supported
                            prF->SetFunction( EContListFunc, EFalse );
                            prF->SetCapability( KImpsServiceGCLI | KImpsServiceCCLI |
                                                KImpsServiceDCLI | KImpsServiceMCLS, EFalse );
                            }
                        else
                            {
                            // Check GCLI
                            myKey->AddL( CREATEKEY( EImpsKeyGCLI, 0 ) );
                            if ( aData->RestoreEmptyL( myKey ) )
                                {
                                prF->SetCapability( KImpsServiceGCLI, EFalse );
                                }
                            // Check CCLI
                            myKey->ReplaceLastL( CREATEKEY( EImpsKeyCCLI, 0 ) );
                            if ( aData->RestoreEmptyL( myKey ) )
                                {
                                prF->SetCapability( KImpsServiceCCLI, EFalse );
                                }
                            // Check DCLI
                            myKey->ReplaceLastL( CREATEKEY( EImpsKeyDCLI, 0 ) );
                            if ( aData->RestoreEmptyL( myKey ) )
                                {
                                prF->SetCapability( KImpsServiceDCLI, EFalse );
                                }
                            // Check MCLS
                            myKey->ReplaceLastL( CREATEKEY( EImpsKeyMCLS, 0 ) );
                            if ( aData->RestoreEmptyL( myKey ) )
                                {
                                prF->SetCapability( KImpsServiceMCLS, EFalse );
                                }
                            myKey->PopL();
                            }
                        }
                    }
                myKey->PopL();
                }
            } // Presence

        // Fundamental

        myKey->ReplaceLastL( CREATEKEY( EImpsKeyFundamentalFeat, 0 ) );
        if ( aData->CheckBranchExistenceL( myKey ) )
            {
            TImpsFeature* fF = aServices->Feature( EFundamentalFeat );
            if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                {
                // Fundamental totally denied
                fF->SetSupported( EFalse );
                fF->SetFunction( EAllFuncs, EFalse );
                fF->SetCapability( 0xFFFFFFFF , EFalse );
                }
            else
                {
                // Check first SearchFunc
                myKey->AddL( CREATEKEY( EImpsKeySearchFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // SearchFunc not supported
                        fF->SetFunction( ESearchFunc, EFalse );
                        fF->SetCapability( KImpsServiceSRCH | KImpsServiceSTSRC, EFalse );
                        }
                    else
                        {
                        // Check SRCH
                        myKey->AddL( CREATEKEY( EImpsKeySRCH , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            fF->SetCapability( KImpsServiceSRCH, EFalse );
                            }
                        // Check STSRC
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeySTSRC , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            fF->SetCapability( KImpsServiceSTSRC, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                // InviteFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyInviteFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // InviteFunc not supported
                        fF->SetFunction( EInviteFunc, EFalse );
                        fF->SetCapability( KImpsServiceINVIT | KImpsServiceCAINV, EFalse );
                        }
                    else
                        {
                        // Check INVIT
                        myKey->AddL( CREATEKEY( EImpsKeyINVIT , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            fF->SetCapability( KImpsServiceINVIT, EFalse );
                            }
                        // Check CAINV
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyCAINV , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            fF->SetCapability( KImpsServiceCAINV, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                myKey->PopL();
                }
            // We are not interested in ServiceFunc now
            } // Fundamental


        // Group
        TImpsFeature* grF = aServices->Feature( EGroupFeat );
        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupFeat, 0 ) );
        if ( aData->CheckBranchExistenceL( myKey ) )
            {
            if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                {
                // Group totally denied
                grF->SetSupported( EFalse );
                grF->SetFunction( EAllFuncs, EFalse );
                grF->SetCapability( 0xFFFFFFFF , EFalse );
                }
            else
                {
                // Check first GroupMgmtFunc
                myKey->AddL( CREATEKEY( EImpsKeyGroupMgmtFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // GroupMgmtFunc not supported
                        grF->SetFunction( EGroupMgmtFunc, EFalse );
                        grF->SetCapability( KImpsServiceCREAG | KImpsServiceDELGR |
                                            KImpsServiceGETGP | KImpsServiceSETGP, EFalse );
                        }
                    else
                        {
                        // Check CREAG
                        myKey->AddL( CREATEKEY( EImpsKeyCREAG , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceCREAG, EFalse );
                            }
                        // Check DELGR
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyDELGR , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceDELGR, EFalse );
                            }
                        // Check GETGP
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGETGP , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceGETGP, EFalse );
                            }
                        // Check SETGP
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeySETGP , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceSETGP, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                // GroupUseFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupUseFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // GroupUseFunc not supported
                        grF->SetFunction( EGroupUseFunc, EFalse );
                        grF->SetCapability( KImpsServiceSUBGCN | KImpsServiceGRCHN, EFalse );
                        }
                    else
                        {
                        // Check SUBGCN
                        myKey->AddL( CREATEKEY( EImpsKeySUBGCN , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceSUBGCN, EFalse );
                            }
                        // Check GRCHN
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyGRCHN , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceGRCHN, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                // GroupAuthFunc
                myKey->ReplaceLastL( CREATEKEY( EImpsKeyGroupAuthFunc, 0 ) );
                if ( aData->CheckBranchExistenceL( myKey ) )
                    {
                    if ( !aData->RestoreAlternativeL( myKey, enumi, index, kType ) )
                        {
                        // GroupAuthFunc not supported
                        grF->SetFunction( EGroupAuthFunc, EFalse );
                        grF->SetCapability( KImpsServiceGETGM | KImpsServiceADDGM |
                                            KImpsServiceRMVGM | KImpsServiceMBRAC | KImpsServiceREJEC, EFalse );
                        }
                    else
                        {
                        // Check GETGM
                        myKey->AddL( CREATEKEY( EImpsKeyGETGM , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceGETGM, EFalse );
                            }
                        // Check ADDGM
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyADDGM , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceADDGM, EFalse );
                            }
                        // Check RMVGM
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyRMVGM , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceRMVGM, EFalse );
                            }
                        // Check MBRAC
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyMBRAC , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceMBRAC, EFalse );
                            }
                        // Check REJEC
                        myKey->ReplaceLastL( CREATEKEY( EImpsKeyREJEC , 0 ) );
                        if ( aData->RestoreEmptyL( myKey ) )
                            {
                            grF->SetCapability( KImpsServiceREJEC, EFalse );
                            }
                        myKey->PopL();
                        }
                    }
                myKey->PopL();
                }
            } // Group


        myKey->PopL();
        }

    CleanupStack::PopAndDestroy( 1 ); // >>> myKey
    }


// ---------------------------------------------------------
// TImpsSrvUtils::CheckServiceL
// ---------------------------------------------------------
void TImpsSrvUtils::CheckServiceL(
    TImpsFeatures aFeature,
    TImpsServices* aSrvS )
    {
    if ( !aSrvS )
        {
        User::Leave( KErrNotSupported );
        }
    TImpsFeature* srvF = aSrvS->Feature( aFeature );
    if ( !srvF->IsSupported() )
        {
        User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------
// TImpsSrvSettings::TImpsSrvSettings
// ---------------------------------------------------------
//
TImpsSrvSettings::TImpsSrvSettings( /*RFs&  aFs*/ )
        :
        // iFs ( aFs ),
        iPollTime( KImpsPollTime ),
        iAliveTime( KImpsAliveTime ),
        iMaxPollTime( KImpsMaxPoll ),
        iXmlLog( 1 ),
        iUDPWAPCIR( 1 ),
        iSMSWAPCIR( 1 ),
        iTCPSCIR( 1 ),
        iUDPSCIR( 1 ),
        iAccessExp( KImpsClientExpr ),
        iImExp( KImpsClientExpr ),
        iPrExp( KImpsPresClientExpr ),
        iGrExp( KImpsClientExpr ),
        iFuExp( KImpsFundClientExpr ),
        iMultiTrans( KImpsMultiTrans ),
        iFourWayLogin( ETrue ),
        iPDPExpiry( KImpsPDPMaxIdleTime ),
        iCIRModePollTime( KImpsCIRModePollTime ),
        iMaxMessageSize( KImpsAcceptedContentLength )
    {}

// ---------------------------------------------------------
// TImpsSrvSettings::ReadStaticSettings
// ---------------------------------------------------------
//
void TImpsSrvSettings::ReadStaticSettings()
    {
    TInt aux = 0;
    // Read Central Repository settings
    TRAPD( errx, aux = TImpsDataUtils::GetCenRepIntValueL( KCRUIDWVEngineVariation, KWVEngineDropContextWhileIdle ) );
    if ( !errx )
        {
        iPDPExpiry = 60 * aux;
        }
    TRAP( errx, iFourWayLogin = TImpsDataUtils::GetCenRepIntValueL(
                                    KCRUIDWVEngineVariation, KWVEngineLoginMethod ) );
    TRAP( errx, iPollTime = TImpsDataUtils::GetCenRepIntValueL(
                                KCRUIDWVEngineVariation, KWVEnginePollTime ) );
    TRAP( errx, iAliveTime = TImpsDataUtils::GetCenRepIntValueL(
                                 KCRUIDWVEngineVariation, KWVEngineDefaultKeepAliveTime ) );
    TRAP( errx, iAccessExp = TImpsDataUtils::GetCenRepIntValueL(
                                 KCRUIDWVEngineVariation, KWVEngineACExpiryTime ) );
    TRAP( errx, iImExp = TImpsDataUtils::GetCenRepIntValueL(
                             KCRUIDWVEngineVariation, KWVEngineIMExpiryTime ) );
    TRAP( errx, iGrExp = TImpsDataUtils::GetCenRepIntValueL(
                             KCRUIDWVEngineVariation, KWVEngineGRExpiryTime ) );
    TRAP( errx, iFuExp = TImpsDataUtils::GetCenRepIntValueL(
                             KCRUIDWVEngineVariation, KWVEngineFUExpiryTime ) );
    TRAP( errx, iPrExp = TImpsDataUtils::GetCenRepIntValueL(
                             KCRUIDWVEngineVariation, KWVEnginePRExpiryTime ) );
    TRAP( errx, iCIRModePollTime = TImpsDataUtils::GetCenRepIntValueL(
                                       KCRUIDWVEngineVariation, KWVEngineCIRModePollTime ) );
    TRAP( errx, iMultiTrans = TImpsDataUtils::GetCenRepIntValueL(
                                  KCRUIDWVEngineVariation, KWVEngineMultiTrans ) );
    TRAP( errx, iMaxMessageSize = TImpsDataUtils::GetCenRepIntValueL(
                                      KCRUIDWVEngineVariation, KWVEngineMaxMessageSize ) );

    // debug logging
#ifdef _IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: ReadSettings iPDPExpiry=%d iFourWayLogin=%d iPollTime=%d iAliveTime=%d" ),
                            iPDPExpiry, iFourWayLogin, iPollTime, iAliveTime );
    CImpsClientLogger::Log( _L( "Server: ReadSettings iAccessExp=%d iImExp=%d iGrExp=%d iFuExp=%d iPrExp=%d iCIRModePollTime=%d iMultiTrans=%d iMaxMsgSize=%d" ),
                            iAccessExp, iImExp, iGrExp, iFuExp, iPrExp, iCIRModePollTime, iMultiTrans, iMaxMessageSize );
#endif
    }

// ---------------------------------------------------------
// TImpsSrvSettings::ReadIniFileL
// ---------------------------------------------------------
//
#pragma CTC SKIP
#ifdef _DEBUG

void TImpsSrvSettings::ReadIniFileL( RFs&  aFs )
    {
    // Set few values without reading them from ini file
    TBool retx = EFalse;
    TInt errx = KErrNone;

    CImpsSettingFile* configFile = CImpsSettingFile::NewL( aFs );
    CleanupStack::PushL( configFile );  // *****
    // Search current drive
    TFileName path;
    Dll::FileName( path );
    TDriveName drive( TParsePtrC( path ).Drive( ) );

    // In emulator we try to find from c drive
    // in hardware we try to find from current drive and then from RAM.
#ifdef __WINS__
    TFileName fileN( KImpsIniFile );
#else
    TFileName fileN( drive );
    fileN.Append( KImpsIniFile );
#endif

    TInt err = 0;

    // Open temporary config file
    TRAP( err, configFile->OpenL( fileN ) );
    if ( err && drive.CompareF( _L( "c:" ) ) )
        {
        err = KErrNone;
        fileN = _L( "c:" );
        fileN.Append( KImpsIniFile );
        TRAP( err, configFile->OpenL( fileN ) );
        }
    if ( err )
        {
        err = KErrNone;
#ifdef _IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: Config file not found ***" ) );
#endif
        User::Leave( KImpsErrorConfig );
        }
    else
        {
#ifdef _IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "Server: Config file found ***" ) );
#endif
        }

    iXmlLog = 0;
    TUint32 myPoll = 0;
    TPtrC8 myPtr;
    TBuf<8> pollBuffer;
    // read poll time
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniPollTime ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myPoll, EDecimal );
        iPollTime = ( TInt ) myPoll;
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniXmlLog ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iXmlLog += 1;
            }
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniXmlPollingLog ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iXmlLog += 2;
            }
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniNoWAPSMSCIR ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iSMSWAPCIR = 0;
            }
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniNoWAPUDPCIR ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iUDPWAPCIR = 0;
            }
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniNoTCPSCIR ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iTCPSCIR = 0;
            }
        }

    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniNoUDPSCIR ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iUDPSCIR = 0;
            }
        }

    // read Keep-Alive-Time
    TUint32 myTime = iAliveTime; // default
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniKeepAlive ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iAliveTime = ( TInt ) myTime;
        }

    // read CIR mode PollTime
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniCIRModePollTime ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iCIRModePollTime = ( TInt ) myTime;
        }

    // read Access expiry time
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniACExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iAccessExp = ( TInt ) myTime;
        }

    // read group expiry
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniGRExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iGrExp = ( TInt ) myTime;
        }

    // read presence expiry
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniPRExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iPrExp = ( TInt ) myTime;
        }

    // read fundamental expiry
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniFUExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iFuExp = ( TInt ) myTime;
        }

    // read IM expiry
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniIMExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iImExp = ( TInt ) myTime;
        }

    // read PDP expiry
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniPDPExpiry ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iPDPExpiry = ( TInt ) myTime;
        }

    // MultiTrans initial
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniMultiTrans ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iMultiTrans = ( TInt ) myTime;
        }

    // 4-way login
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIni4WayLogin ) ) );
    if ( !err )
        {
        if ( !myPtr.CompareF( _L8( "False" ) ) )
            {
            iFourWayLogin = EFalse;
            }
        else if ( !myPtr.CompareF( _L8( "True" ) ) )
            {
            iFourWayLogin = ETrue;
            }
        }

    // Maximum IM message size
    myTime = 0;
    TRAP( err, myPtr.Set( configFile->KeyValueL( KImpsIniMaxMessageSize ) ) );
    if ( !err )
        {
        pollBuffer.Copy( myPtr );
        TLex lex;
        lex.Assign( pollBuffer );
        lex.Val( myTime, EDecimal );
        iMaxMessageSize = ( TInt ) myTime;
        }

    CleanupStack::PopAndDestroy( 1 );  // configFile

    // debug logging
#ifdef _IMPS_LOGGING_
    CImpsClientLogger::Log( _L( "Server: ReadFile iPDPExpiry=%d iFourWayLogin=%d iPollTime=%d iAliveTime=%d" ),
                            iPDPExpiry, iFourWayLogin, iPollTime, iAliveTime );
    CImpsClientLogger::Log( _L( "Server: ReadFile iAccessExp=%d iImExp=%d iGrExp=%d iFuExp=%d iPrExp=%d iCIRModePollTime=%d iMultiTrans=%d iMaxMsgSize=%d" ),
                            iAccessExp, iImExp, iGrExp, iFuExp, iPrExp, iCIRModePollTime, iMultiTrans, iMaxMessageSize );
    CImpsClientLogger::Log( _L( "Server: ReadFile \
        iXmlLog=%d, iSMSWAPCIR=%d, iUDPWAPCIR=%d,iTCPSCIR=%d,iUDPSCIR=%d" ),
                            iXmlLog, iSMSWAPCIR, iUDPWAPCIR, iTCPSCIR, iUDPSCIR );
#endif
    }

#endif // _DEBUG    
#pragma CTC ENDSKIP

// ---------------------------------------------------------
// TImpsSrvSettings::SetCirBindingsL
// ---------------------------------------------------------
//
void TImpsSrvSettings::SetCirBindingsL(
    MImpsDataAccessor* myAccess, MImpsKey* myKey )
    {
    TBool everyElement( EFalse );
    TInt myInt = 0;

    // Check supported CIRMethods
    for ( TInt i = 0; !everyElement; i++ )
        {
        myKey->AddL( CREATEKEY( EImpsKeySupportedCIRMethod, i ) );
        if ( myAccess->RestoreIntegerL( myKey, myInt ) )
            {
            if ( myInt == EImpsWAPUDP )
                {
                iUDPWAPCIR++;
                }
            else if (  myInt == EImpsWAPSMS )
                {
                iSMSWAPCIR++;
                }
            else if (  myInt == EImpsSUDP )
                {
                iUDPSCIR++;
                }
            else if (  myInt == EImpsSTCP )
                {
                iTCPSCIR++;
                }
            }
        else
            {
            // All methods handled
            everyElement = ETrue;
            }
        myKey->PopL();
        }
    }


// ---------------------------------------------------------
// TImpsSessIdent::TImpsSessIdent
// ---------------------------------------------------------
//
TImpsSessIdent::TImpsSessIdent(
    const TDesC& aSID, const TDesC& aSAP, const TDesC& aUserId )
        : iSID( aSID ), iSAP( aSAP ), iUserId( aUserId )
    {}



//  End of File
