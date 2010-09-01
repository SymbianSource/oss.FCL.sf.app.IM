/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Access handler of the network session
*
*/

// INCLUDE FILES

#include    <ImpsServices.h>
#include    "MPEngSessionStatusObserver.h"
#include    "CImpsPluginAccessHandler.h"
#include    "PEngWVPresenceErrors2.h"

#include    "impsDebugPrint.h"
#include    "impsconfig.h"
#include    "impspluginPanics.h"

//DATA TYPES
/**
 * Function map structure.
 */
struct SCspFunctionMap
    {
    TInt           iPeFunc;
    TImpsFunctions iImpsEngFunc;

#ifdef IMPS_ENABLE_DEBUG_PRINT
    const TText*   iPeFuncName;
#endif
    };


/**
 * Sub-Function map structure.
 */
struct SCspSubFunctionMap
    {
    TInt           iPeSubFunc;
    TInt           iImpsEngSubFunc;

#ifdef IMPS_ENABLE_DEBUG_PRINT
    const TText*   iPeSubFuncName;
#endif
    };

//CONTANTS
const TInt KPEngNullFunction = 0;


//MACROS
#ifdef IMPS_ENABLE_DEBUG_PRINT
#define MAPPING( a1, a2 ) { a1, a2, _S( #a1 ) }
#define MAPPING_TERMINATION { KPEngNullFunction }
#else
#define MAPPING( a1, a2 ) { a1, a2 }
#define MAPPING_TERMINATION { KPEngNullFunction }
#endif





/**
 * Fundamental functions & sub-functions.
 */
const SCspFunctionMap KFuncMapFundamental[] =
    {
    MAPPING( KPEngFFServiceFunction, EServiceFunc ),
    MAPPING( KPEngFFSearchFunction,  ESearchFunc  ),
    MAPPING( KPEngFFInviteFunction,  EInviteFunc  ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapFundamental[] =
    {
    MAPPING( KPEngFFSubFuncGETSPI,   KImpsServiceGETSPI ),
    MAPPING( KPEngFFSubFuncSRCH,     KImpsServiceSRCH   ),
    MAPPING( KPEngFFSubFuncSTSRC,    KImpsServiceSTSRC  ),
    MAPPING( KPEngFFSubFuncINVIT,    KImpsServiceINVIT  ),
    MAPPING( KPEngFFSubFuncCAINV,    KImpsServiceCAINV  ),
    MAPPING_TERMINATION
    };

/**
 * Presence functions & sub-functions.
 */
const SCspFunctionMap KFuncMapPresence[] =
    {
    MAPPING( KPEngWVContListFunction,  EContListFunc        ),
    MAPPING( KPEngWVPresAuthFunction,  EPresenceAuthFunc    ),
    MAPPING( KPEngWVPresDelivFunction, EPresenceDeliverFunc ),
    MAPPING( KPEngWVAttrListFunction,  EAttrListFunc        ),
    MAPPING_TERMINATION
    };


const SCspSubFunctionMap KSubFuncMapPresence[] =
    {
    MAPPING( KPEngWVSubFuncGCLI,   KImpsServiceGCLI  ),
    MAPPING( KPEngWVSubFuncCCLI,   KImpsServiceCCLI  ),
    MAPPING( KPEngWVSubFuncDCLI,   KImpsServiceDCLI  ),
    MAPPING( KPEngWVSubFuncMCLS,   KImpsServiceMCLS  ),
    MAPPING( KPEngWVSubFuncGETWL,  KImpsServiceGETWL ),
    MAPPING( KPEngWVSubFuncREACT,  KImpsServiceREACT ),
    MAPPING( KPEngWVSubFuncCAAUT,  KImpsServiceCAAUT ),
    //{ KPEngWVSubFuncGETAUT, Not yet in WV Engine },
    MAPPING( KPEngWVSubFuncGETPR,  KImpsServiceGETPR ),
    MAPPING( KPEngWVSubFuncUPDPR,  KImpsServiceUPDPR ),
    MAPPING( KPEngWVSubFuncCALI,   KImpsServiceCALI  ),
    MAPPING( KPEngWVSubFuncDALI,   KImpsServiceDALI  ),
    MAPPING( KPEngWVSubFuncGALS,   KImpsServiceGALS  ),
    MAPPING_TERMINATION
    };

/**
 * IM Functions & sub-functions.
 */
const SCspFunctionMap KFuncMapIm[] =
    {
    MAPPING( KPEngIMSendFunctions,     EIMSendFunc    ),
    MAPPING( KPEngIMReceiveFunctions,  EIMReceiveFunc ),
    MAPPING( KPEngIMAuthorFunctions,   EIMAuthFunc    ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapIm[] =
    {
    // Send Functions
    MAPPING( KPEngIMSubFuncMDELIV, KImpsServiceMDELIV ),
    MAPPING( KPEngIMSubFuncFWMSG,  KImpsServiceFWMSG  ),
    // Receive functions
    MAPPING( KPEngIMSubFuncSETD,   KImpsServiceSETD   ),
    MAPPING( KPEngIMSubFuncGETLM,  KImpsServiceGETLM  ),
    MAPPING( KPEngIMSubFuncGETM,   KImpsServiceGETM   ),
    MAPPING( KPEngIMSubFuncREJCM,  KImpsServiceREJCM  ),
    MAPPING( KPEngIMSubFuncNOTIF,  KImpsServiceNOTIF  ),
    MAPPING( KPEngIMSubFuncNEWM,   KImpsServiceNEWM   ),
    // Authorize functions
    MAPPING( KPEngIMSubFuncGLBLU,  KImpsServiceGLBLU  ),
    MAPPING( KPEngIMSubFuncBLENT,  KImpsServiceBLENT  ),
    MAPPING_TERMINATION
    };

/**
 * Group Functions & sub-functions.
 */
/**
* IM Functions & sub-functions.
*/
const SCspFunctionMap KFuncMapGroup[] =
    {
    MAPPING( KPEngGRManageFunctions, EGroupMgmtFunc ),
    MAPPING( KPEngGRUseFunctions,    EGroupUseFunc  ),
    MAPPING( KPEngGRAuthorFunctions, EGroupAuthFunc ),
    MAPPING_TERMINATION
    };

const SCspSubFunctionMap KSubFuncMapGroup[] =
    {
    // Manager functions
    MAPPING( KPEngGRSubFuncCREAG,  KImpsServiceCREAG  ),
    MAPPING( KPEngGRSubFuncDELGR,  KImpsServiceDELGR  ),
    MAPPING( KPEngGRSubFuncGETGP,  KImpsServiceGETGP  ),
    MAPPING( KPEngGRSubFuncSETGP,  KImpsServiceSETGP  ),
    // Use functions
    MAPPING( KPEngGRSubFuncSUBGCN, KImpsServiceSUBGCN ),
    MAPPING( KPEngGRSubFuncGRCHN,  KImpsServiceGRCHN  ),
    // Authorization functions
    MAPPING( KPEngGRSubFuncGETGM,  KImpsServiceGETGM  ),
    MAPPING( KPEngGRSubFuncADDGM,  KImpsServiceADDGM  ),
    MAPPING( KPEngGRSubFuncRMVGM,  KImpsServiceRMVGM  ),
    MAPPING( KPEngGRSubFuncMBRAC,  KImpsServiceMBRAC  ),
    MAPPING( KPEngGRSubFuncREJEC,  KImpsServiceREJEC  ),
    MAPPING_TERMINATION
    };


// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DebugDumpFeatureSubtree()
// -----------------------------------------------------------------------------
//
#ifdef IMPS_ENABLE_DEBUG_PRINT
void DebugDumpFeatureSubtree( const TDesC& aFeatureName,
                              TPEngWVCspFeature2& aPEFeature,
                              const SCspFunctionMap aFunctionMap[],
                              const SCspSubFunctionMap aSubFunctionMap[] )

    {
    //Whole feature
    IMPS_DP( D_IMPS_LIT( "%S, support status: %d" ),
             &aFeatureName,  aPEFeature.FeatureSupported() );

    //Functions
    IMPS_DP( D_IMPS_LIT( "%S, supported functions:" ), &aFeatureName );
    for ( TInt i( 0 ); aFunctionMap[ i ].iPeFunc != KPEngNullFunction; ++i )
        {
        const SCspFunctionMap functionMap = aFunctionMap[ i ];

        if ( aPEFeature.FunctionSupported( functionMap.iPeFunc ) )
            {
            TPtrC peFuncName( functionMap.iPeFuncName );
            IMPS_DP( D_IMPS_LIT( "   %S" ), &peFuncName );
            }
        }


    //Sub-functions
    IMPS_DP( D_IMPS_LIT( "%S, supported sub-functions:" ), &aFeatureName );
    for ( TInt ii( 0 ); aSubFunctionMap[ ii ].iPeSubFunc != KPEngNullFunction; ++ii )
        {
        const SCspSubFunctionMap subFuncMap = aSubFunctionMap[ ii ];
        if ( aPEFeature.SubFunctionSupported( subFuncMap.iPeSubFunc ) )
            {
            TPtrC peSubFuncName( subFuncMap.iPeSubFuncName );
            IMPS_DP( D_IMPS_LIT( "   %S" ), &peSubFuncName );
            }
        }
    }
#endif


// -----------------------------------------------------------------------------
// CopyFeatureSubtree()
// -----------------------------------------------------------------------------
//
void CopyFeatureSubtree( TPEngWVCspFeature2& aPEFeature,
                         TImpsFeature& aWVFeature,
                         const SCspFunctionMap aFunctionMap[],
                         const SCspSubFunctionMap aSubFunctionMap[] )

    {
    // Is feature supported at all
    aPEFeature.iSupported = aWVFeature.IsSupported();

    TInt holder;

    //Functions
    holder = KPEngNullFunction;
    for ( TInt i( 0 ); aFunctionMap[ i ].iPeFunc != KPEngNullFunction; ++i )
        {
        const SCspFunctionMap functionMap = aFunctionMap[ i ];
        if ( aWVFeature.Function( functionMap.iImpsEngFunc ) )
            {
            holder |=  functionMap.iPeFunc;
            }
        }
    aPEFeature.iFunctions = holder;

    //Sub-functions
    holder = KPEngNullFunction;
    for ( TInt ii( 0 ); aSubFunctionMap[ ii ].iPeSubFunc != KPEngNullFunction; ++ii )
        {
        const SCspSubFunctionMap subFunctionMap = aSubFunctionMap[ ii ];
        if ( aWVFeature.Capability( subFunctionMap.iImpsEngSubFunc ) )
            {
            holder = holder + subFunctionMap.iPeSubFunc;
            }
        }
    aPEFeature.iSubFunctions = holder;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::CImpsPluginAccessHandler
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CImpsPluginAccessHandler::CImpsPluginAccessHandler()
        : iSessStatus( EPEngNWPresenceSessionClosed ),
        iOperation( EIdle )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::CImpsPluginAccessHandler Start End" ) );
    }

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::ConstructL( RImpsEng& aSessClient )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::ConstructL() Start" ) );

    iAccessClient.RegisterL( aSessClient, this );
    iAccessClient.RegisterErrorObserverL( *this );
    iAccessClient.RegisterStatusObserverL( this );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::ConstructL() End" ) );
    }

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImpsPluginAccessHandler* CImpsPluginAccessHandler::NewL( RImpsEng& aSessClient )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::NewL() Start" ) );

    CImpsPluginAccessHandler* self = new ( ELeave ) CImpsPluginAccessHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aSessClient );
    CleanupStack::Pop( self );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::NewL() End" ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEngAccessHandler::~CImpsPluginAccessHandler
// Destructor
// -----------------------------------------------------------------------------
//

CImpsPluginAccessHandler::~CImpsPluginAccessHandler()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::~CImpsPluginAccessHandler() Start" ) );

    __ASSERT_DEBUG( ( iObservers.Count() == 0 ), User::Panic(
                        KImpsPluginPanic, EOpIdConsistencyLost ) );

    iAccessClient.Unregister();

    iObservers.Reset();
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::~CImpsPluginAccessHandler() Start" ) );
    }


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// =============================================================================
// =============== New Functions of the MImpsAccessHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::LogInL(
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::LogInL( TRequestStatus& aStatus,
                                       TDesC &aPassword,
                                       TDesC &aClientId,
                                       TDesC &aServiceAddress,
                                       TInt32 aSapId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()  Start" ) );

    // is busy?
    if ( iOperation != EIdle )
        {
        // client is busy, cancel the request

        IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()  Start" ) );

        aStatus = KRequestPending;
        aStatus = KErrNotReady;
        iStatus = &aStatus;
        iOperation = EDoingLogIn;
        Cancel( KErrNotReady );
        return;
        // User::Leave( KErrNotReady );
        // send status back to prfw that connection is busy
        }

    // Are we logged?
    if ( iSessStatus == EPEngNWPresenceSessionOpen )
        {
        // we are already logged, this should not happened
        // cancel the request
        aStatus = KRequestPending;
        aStatus = KErrAlreadyExists;
        iStatus = &aStatus;
        iOperation = EDoingLogIn;
        Cancel( KErrAlreadyExists );
        return;
        //User::Leave( KErrAlreadyExists );
        // do we need send request with error or normal completetion?
        }

    IMPS_DP( D_IMPS_LIT( "-----------------------------------------------------" ) );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL() - doing reactive login" ) );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()Password:%S " ), &aPassword );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()UserId:%S " ), &aClientId );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()ServiceAddress:%S " ), &aServiceAddress );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL()Accesspoint:%d" ), &aSapId );

    TImpsCspIdentifier sessIndentifier;
    sessIndentifier.SetUserIdL( aClientId );
    sessIndentifier.SetSapL( aServiceAddress );

    // panic if we get other session log in data, user's gone wacko

    iOpId = iAccessClient.ReactiveLoginL(
                sessIndentifier,                    // Identifier
                aPassword,							// sapSetting->SAPUserPassword(),      // User Password
                aClientId, 							// sapSetting->ClientId(),             // Client Id
                aSapId,								// sapSetting->AccessPoint(),          // Access Point
                NULL,      							// Operator key 1
                NULL ); 						    	// Operator key 2

    // log in issued, set operation to the Log In

    aStatus = KRequestPending;
    iStatus = &aStatus;
    iOperation = EDoingLogIn;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogInL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::LogOutL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::LogOutL( TRequestStatus& aStatus )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogOutL() Start" ) );

    if ( iOperation != EIdle )
        {
        // client is busy, leave
        //User::Leave( KErrNotReady );

        IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogOutL() operation != EIdle" ) );

        aStatus = KRequestPending;
        aStatus = KErrNotReady;
        iStatus = &aStatus;
        iOperation = EDoingLogOut;
        Cancel( KErrNotReady );
        return;
        }

    // Are we even logged
    if ( iSessStatus != EPEngNWPresenceSessionOpen )
        {

        // we are already logged out
        //return;

        IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogOutL() iSessStatus != EPEngNWPresenceSessionOpen" ) );

        aStatus = KRequestPending;
        aStatus = KErrCancel;
        iStatus = &aStatus;
        iOperation = EDoingLogOut;
        Cancel( KErrCancel );
        return;

        }

    iOpId = iAccessClient.LogoutL();
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iOperation = EDoingLogOut;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::LogOutL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::CancelL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::Cancel( TInt aReason )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::Cancel, Status:%d operation:%d" ),
             iSessStatus,
             iOperation  );
    if ( iStatus )
        {
        switch ( iOperation )
            {
            case EDoingLogIn:
                {
                // cancel login
                TRAP_IGNORE( iAccessClient.CancelLoginL( iOpId ) );
                // cancel login with reason
                //User::RequestComplete( iStatus, KErrCancel );
                User::RequestComplete( iStatus, aReason );
                iStatus = NULL;
                iOperation = ECanceling;
                break;
                }
            case EDoingLogOut:
                {
                // cancel logout
                // no cancel, complete and let it delete
                User::RequestComplete( iStatus, KErrCancel );
                iStatus = NULL;
                iOperation = ECanceling;
                break;
                }
            default:
                {
                // this should not happen, active, status, and idle operation
                // completed status
                User::RequestComplete( iStatus, KErrCancel );
                iStatus = NULL;
                }
            }
        }

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::Cancel() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::RegisterSessionObserver(
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::RegisterSessionObserverL( MPEngSessionStatusObserver& aObserver )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::RegisterSessionObserverL() Start" ) );
    if ( KErrNotFound != iObservers.Find( &aObserver ) )
        {
        User::Leave( KErrAlreadyExists );
        }
    iObservers.AppendL( &aObserver );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::RegisterSessionObserverL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::UnregisterSessionObserver(
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::UnregisterSessionObserver( MPEngSessionStatusObserver& aObserver )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::UnregisterSessionObserver() Start" ) );
    TInt index ( iObservers.Find( &aObserver ) );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::UnregisterSessionObserver() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::ServiceCapabilities()
// -----------------------------------------------------------------------------
//
TPEngWVCspServicesTree2& CImpsPluginAccessHandler::ServiceCapabilities()
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::ServiceCapabilities() Start-End" ) );
    return iServices;
    }


// =============================================================================
// =============== New Functions of the MImpsAccessHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleLoginL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleLoginL(
    TInt aId,
    TImpsCspIdentifier& aCspId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLoginL - id:  %d" ), aId );

    IMPSPRTPLUGIN_UNUSED_PARAM( aCspId );

    // panic if we get other session notification

    // if operation came from some other session it should be 0
    __ASSERT_DEBUG( ( aId == iOpId ), User::Panic(
                        KImpsPluginPanic, EOpIdConsistencyLost ) );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLoginL End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleLogoutL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleLogoutL(
    TInt aId,
    TImpsCspIdentifier& aCspId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLogoutL - id:  %d" ), aId );

    IMPSPRTPLUGIN_UNUSED_PARAM( aCspId );

    // panic if we get other session notification

    TPEngNWSessionSlotEvent event;
    // Operation id -1 means that server kicked us out
    if ( aId == -1 )
        {
        event = EPEngEventNWSessionClosedByServer;
        }
    else
        {
        event = EPEngEventAppNWPresenceSessionClosed;
        }

    if ( iStatus )
        {
        if ( iOperation == EDoingLogOut )
            {
            User::RequestComplete( iStatus, KErrNone );
            }
        else
            {
            User::RequestComplete( iStatus, KErrCancel );
            }
        iStatus = NULL;
        iOperation = EIdle;
        iOpId = KErrNone;
        }

    // notify observers
    NotifyIfNewStateL( EPEngNWPresenceSessionClosed, event );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLogoutL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleLoginCancelL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleLoginCancelL(
    TInt aCancelledOpId,
    TImpsCspIdentifier& aCspId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLoginCancelL - id:  %d" ),
             aCancelledOpId );

    IMPSPRTPLUGIN_UNUSED_PARAM( aCspId );

    // panic if we get other session notification

    __ASSERT_DEBUG( ( aCancelledOpId == iOpId ),
                    User::Panic( KImpsPluginPanic, EOpIdConsistencyLost ) );

    iSessStatus = EPEngNWPresenceSessionClosed;
    iOperation = EIdle;
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLoginCancelL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleNbrSessionsL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleNbrSessionsL(
    TInt /* aId */,
    TInt /* aNbr */,
    TImpsCspIdentifier& /* aCspId */ )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleNbrSessionsL() Start-End" ) );
    }


// =============================================================================
// =============== New Functions of the MImpsErrorHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleErrorL(
    TInt aStatus,
    TInt /*aOpId*/,
    const TDesC* /* aDescription */,
    const CImpsDetailed* /* aDetailedRes */,
    TImpsCspIdentifier& aCspId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleErrorL - id:  %d" ), aStatus );
    // panic if we get other session notification

    IMPSPRTPLUGIN_UNUSED_PARAM( aCspId );

    // we get here if service negotiation failed
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleErrorL  Before If Statement " ) );
    if ( iStatus )
        {
        IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleErrorL  Inside If loop " ) );
        iOperation = EIdle;
        User::RequestComplete( iStatus, aStatus );
        iStatus = NULL;
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleErrorL  After If Statement " ) );

    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleErrorL() End" ) );
    }


// =============================================================================
// =============== New Functions of the MImpsStatusHandler =====================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleStatusChangeL()
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleStatusChangeL(
    TImpsServiceStatus aStatus,
    TImpsCspIdentifier& aCspId )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleStatusChangeL:  %d" ), aStatus );
    // panic if we get other session notification
    IMPSPRTPLUGIN_UNUSED_PARAM( aCspId );

    switch ( aStatus )
        {
        case EImps_ON_LINE:
            {
            HandleLogInCompletionL();
            break;
            }
        case EImps_NOT_LOGGED:
            {
            NotifyIfNewStateL( EPEngNWPresenceSessionClosed,
                               EPEngEventAppNWPresenceSessionClosed );
            iServiceFetched = EFalse;
            break;
            }
        case EImps_SHUTTING_DOWN:
            {
            NotifyIfNewStateL( EPEngNWPresenceSessionClosed,
                               EPEngEventNWSessionClosedByServer );
            iServiceFetched = EFalse;
            break;
            }
        default:
            {
            // do nothing
            User::Panic( KImpsPluginPanic, EUnknownUnumeration );
            }
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleStatusChangeL() End" ) );
    }


// =============================================================================
// =============== New Private Functions of main class =========================
// =============================================================================

// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::CopyTree()
// Copy service tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::CopyServiceTree(
    TImpsServices& aFrom )
    {
    IMPS_DP( D_IMPS_LIT( "Network session capability details:" ) );
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::CopyServiceTree() Start" ) );

    TImpsFeature* wvFeature = NULL;
    // fundamental features
    wvFeature = aFrom.Feature( EFundamentalFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iFundamentalFeat,
                            *wvFeature,
                            KFuncMapFundamental,
                            KSubFuncMapFundamental );
#ifdef IMPS_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "FundamentalFeatures" ), // CSI: 78 #
                                 iServices.iFundamentalFeat,
                                 KFuncMapFundamental,
                                 KSubFuncMapFundamental );
#endif
        }

    wvFeature = NULL;
    // Presence features
    wvFeature = aFrom.Feature( EPresenceFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iPresenceFeat,
                            *wvFeature,
                            KFuncMapPresence,
                            KSubFuncMapPresence );
#ifdef IMPS_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "PresenceFeatures" ), // CSI: 78 #
                                 iServices.iPresenceFeat,
                                 KFuncMapPresence,
                                 KSubFuncMapPresence );
#endif
        }

    wvFeature = NULL;
    // Im features
    wvFeature = aFrom.Feature( EIMFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iIMFeat,
                            *wvFeature,
                            KFuncMapIm,
                            KSubFuncMapIm );
#ifdef IMPS_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "IMFeatures" ), // CSI: 78 #
                                 iServices.iIMFeat,
                                 KFuncMapIm,
                                 KSubFuncMapIm );
#endif
        }

    wvFeature = NULL;
    // Group features
    wvFeature = aFrom.Feature( EGroupFeat );
    if ( wvFeature )
        {
        CopyFeatureSubtree( iServices.iGroupFeat,
                            *wvFeature,
                            KFuncMapGroup,
                            KSubFuncMapGroup );
#ifdef IMPS_ENABLE_DEBUG_PRINT
        DebugDumpFeatureSubtree( _L( "GroupFeatures" ), // CSI: 78 #
                                 iServices.iGroupFeat,
                                 KFuncMapGroup,
                                 KSubFuncMapGroup );
#endif
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::CopyServiceTree() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::NotifyIfNewStateL()
// Notify if new state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::NotifyIfNewStateL(
    TPEngNWSessionSlotState aNewState,
    TPEngNWSessionSlotEvent aEvent )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::NotifyIfNewStateL() Start" ) );
    if ( iSessStatus != aNewState )
        {
        iSessStatus = aNewState;
        for ( TInt x( iObservers.Count() - 1 ) ; x >= 0 ; --x )
            {
            iObservers[ x ]->StatusChangedL( iSessStatus, aEvent );
            }
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::NotifyIfNewStateL() End" ) );
    }


// -----------------------------------------------------------------------------
// CImpsPluginAccessHandler::HandleLogInCompletionL()
// Handle completion of the log in request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CImpsPluginAccessHandler::HandleLogInCompletionL( )
    {
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLogInCompletionL() Start" ) );
    if ( !iServiceFetched )
        {
        // copy tree of capabilities
        TImpsServices services;
        iAccessClient.GetServicesL( services );
        CopyServiceTree( services );
        iServiceFetched = ETrue;
        }

    NotifyIfNewStateL( EPEngNWPresenceSessionOpen,
                       EPEngEventAppNWPresenceSessionOpened );

    if ( iStatus )
        {
        // handle Operation
        iOperation = EIdle;
        User::RequestComplete( iStatus, KErrNone );
        iStatus = NULL;
        iOpId = KErrNone;
        }
    IMPS_DP( D_IMPS_LIT( "CImpsPluginAccessHandler::HandleLogInCompletionL() End" ) );
    }

// End of File


