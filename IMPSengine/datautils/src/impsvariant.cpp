/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handler for bldvariants
*
*/



// INCLUDE FILES
#include    <featmgr.h>
#include    <WvEngineVariant.hrh>

#include    "WVEnginePrivateCRKeys.h"
#include    <centralrepository.h>

#include    "impsvariantAPI.h"
#include    "impsvariant.h"

#include    "impsutils.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImpsVariant::CImpsVariant
// ----------------------------------------------------------------------------
//
CImpsVariant::CImpsVariant ()
    {
    }

// ----------------------------------------------------------------------------
// CImpsVariant::ConstructL
// ----------------------------------------------------------------------------
//
void CImpsVariant::ConstructL()
    {
    iPointerPackage =
        reinterpret_cast<TThreadLocalStoragePackage*> ( Dll::Tls( ) );
    if ( iPointerPackage == NULL )
        {
        // create pointerPackage and store it into Tls
        iPointerPackage = new ( ELeave ) TThreadLocalStoragePackage;
        iPointerPackage->iFeatureBitMask = 0;
        iPointerPackage->iUserCount = 0;
        User::LeaveIfError( Dll::SetTls( iPointerPackage ) );
        }

    if ( iPointerPackage->iUserCount <= 0 )
        {
        // Initialize feature manager
        FeatureManager::InitializeLibL( );
        }
    iPointerPackage->iFeatureBitMask = 0;
    //////////////////////////////////////////////////////////////
    /*    TInt value( 0 );
        CRepository* repository = CRepository::NewLC( KCRUIDWVEngine ); // << repository
        User::LeaveIfError( repository->Get( KWVEngineFeatWapPushOverUDP, value ) );
        if ( value )
            {
            iPointerPackage->iFeatureBitMask = iPointerPackage->iFeatureBitMask & KWVEngineFeatWapPushOverUDP;
            }
        value  = 0;
        User::LeaveIfError( repository->Get( KWVEngineFeatOpenAPILauncher, value ) );
        if ( value )
            {
            iPointerPackage->iFeatureBitMask = iPointerPackage->iFeatureBitMask & KWVEngineFeatOpenAPILauncher;
            }
        CleanupStack::PopAndDestroy(); // >> repository
    */
    //////////////////////////////////////////////////////////////////
    // CHANGED
    CRepository* repository = NULL;
    TInt err( KErrNone );
    TInt value1( 1 );
    TInt value2( 1 );
    TRAP( err, repository = CRepository::NewL( KCRUIDWVEngine ) );
    if ( err == KErrNone )
        {
        CleanupStack::PushL( repository );
        TInt err2 = repository->Get( KWVEngineFeatWapPushOverUDP, value1 ) ;
        if ( err2 != KErrNone )
            {
            value1 = 1;
            }
        TInt err3 = repository->Get( KWVEngineFeatOpenAPILauncher, value2 ) ;
        if ( err3 != KErrNone )
            {
            value2 = 1;
            }
        CleanupStack::PopAndDestroy();
        }
    else
        {
#ifndef _NO_IMPS_LOGGING_
        CImpsClientLogger::Log( _L( "CImpsVariant::ConstructL(): KCRUIDWVEngine not found from cenrep!!!" ) );
#endif
        }
    repository = NULL;
    if ( value1 )
        {
        iPointerPackage->iFeatureBitMask = iPointerPackage->iFeatureBitMask | KWVEngineFeatWapPushOverUDP;
        }
    if ( value2 )
        {
        iPointerPackage->iFeatureBitMask = iPointerPackage->iFeatureBitMask | KWVEngineFeatOpenAPILauncher;
        }
    // END OF CHANGED
    ++iPointerPackage->iUserCount;
    User::LeaveIfError( Dll::SetTls( iPointerPackage ) );
    }

// ----------------------------------------------------------------------------
// CImpsVariant::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CImpsVariant* CImpsVariant::NewLC()
    {
    CImpsVariant* self = new ( ELeave ) CImpsVariant;

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImpsVariant::~CImpsVariant
// ----------------------------------------------------------------------------
//
CImpsVariant::~CImpsVariant()
    {
    iPointerPackage =
        reinterpret_cast<TThreadLocalStoragePackage*> ( Dll::Tls( ) );
    if ( iPointerPackage )
        {
        --iPointerPackage->iUserCount;
        if ( iPointerPackage->iUserCount <= 0 )
            {
            FeatureManager::UnInitializeLib( );
            delete iPointerPackage;
            Dll::SetTls( NULL );
            }
        else
            {
            Dll::SetTls( iPointerPackage );
            }
        }
    }

// ----------------------------------------------------------------------------
// CImpsVariant::IsFeatureSupportedL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CImpsVariant::IsFeatureSupportedL( TImpsVariants aVariant ) const
    {
    TBool ret( EFalse );
// these are used for testing purposes
#ifdef _USEDYNAMICPHONEBOOK
    if ( aVariant == EDpb )
        {
        return ETrue;
        }
#endif
#ifdef _STANDALONE_IP_CIR
    if ( aVariant == EIpCirStandalone )
        {
        return ETrue;
        }
#endif
#ifdef _WAPUDP_CIR
    if ( aVariant == EWAPUDP )
        {
        return ETrue;
        }
#endif
#ifdef _OPENAPI
    if ( aVariant == EOpenAPI )
        {
        return ETrue;
        }
#endif

    switch ( aVariant )
        {
        case EDpb:
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdPresence ) )
                {
                ret = ETrue;
                }
            }
        break;
        case EIpCirStandalone:
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdImpsStandaloneIpCir ) )
                {
                ret =  ETrue;
                }
            }
        break;
        case EWAPUDP:
            {
            if ( iPointerPackage->iFeatureBitMask & KWvEngFeatWapPushOverUDP )
                {
                ret =  ETrue;
                }
            }
        break;
        case EImLauncher:
            {
            if ( iPointerPackage->iFeatureBitMask & KWVEngFeatOpenAPILauncher )
                {
                ret =  ETrue;
                }
            }
        break;
        default:
            // EOpenAPI not supported
            ret =  EFalse;
            break;
        }
    return ret;
    }

//  End of File


