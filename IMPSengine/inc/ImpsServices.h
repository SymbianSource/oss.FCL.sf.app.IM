/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     WV Services tree data structures for Service Negotiation.
*     TImpsServices and TImpsFeature.
*
*/


#ifndef CImpsServices_H
#define CImpsServices_H

//  INCLUDES
#include <e32base.h>
#include "impsconst.h"


// *** FUNCTIONS ***

// Fundamental functions
const TInt KImpsServiceFunc     = 0x0001;
const TInt KImpsSearchFunc      = 0x0002;
const TInt KImpsInviteFunc      = 0x0004;

// Presence functions
const TInt KImpsContListFunc    = 0x0008;
const TInt KImpsPresAuthFunc    = 0x0010;
const TInt KImpsPresDeliverFunc = 0x0020;
const TInt KImpsAttListFunc     = 0x0040;

// IM Functions
const TInt KImpsIMSendFunc      = 0x0080;
const TInt KImpsIMReceiveFunc   = 0x0100;
const TInt KImpsIMAuthFunc      = 0x0200;

// Group functions
const TInt KImpsGroupMgmtFunc   = 0x0400;
const TInt KImpsGroupUseFunc    = 0x0800;
const TInt KImpsGroupAuthFunc   = 0x1000;

// Service value bitmap flags
// Use plus operation or method SetThisFlag to build a set of methods and
// bit mask or method IsThisFlagSet to check if a single method is supported.

const TInt KImpsServiceNone         = 0x0000;
const TInt KAllFuncs                = 0xFFFFFFFF;

// *** CAPABILITIES ***
// Named also sub-functions or elements in CSP definition

// Fundamental capability flags 
const TInt KImpsServiceGETSPI       = 0x0001;
const TInt KImpsServiceSRCH         = 0x0002;
const TInt KImpsServiceSTSRC        = 0x0004;
const TInt KImpsServiceINVIT        = 0x0008;
const TInt KImpsServiceCAINV        = 0x0010;

// Presence capability flags
const TInt KImpsServiceGCLI         = 0x0001;
const TInt KImpsServiceCCLI         = 0x0002;
const TInt KImpsServiceDCLI         = 0x0004;
const TInt KImpsServiceMCLS         = 0x0008;
const TInt KImpsServiceGETWL        = 0x0010;
const TInt KImpsServiceREACT        = 0x0020;
const TInt KImpsServiceCAAUT        = 0x0040;
const TInt KImpsServiceGETPR        = 0x0080;
const TInt KImpsServiceUPDPR        = 0x0100;
const TInt KImpsServiceCALI         = 0x0200;
const TInt KImpsServiceDALI         = 0x0400;
const TInt KImpsServiceGALS         = 0x0800;
const TInt KImpsServiceGETAUT       = 0x1000;  // WV 1.2

// IM capability flags
const TInt KImpsServiceMDELIV       = 0x0001;
const TInt KImpsServiceFWMSG        = 0x0002;
const TInt KImpsServiceSETD         = 0x0004;
const TInt KImpsServiceGETLM        = 0x0008;
const TInt KImpsServiceGETM         = 0x0010;
const TInt KImpsServiceREJCM        = 0x0020;
const TInt KImpsServiceNOTIF        = 0x0040;
const TInt KImpsServiceNEWM         = 0x0080;
const TInt KImpsServiceGLBLU        = 0x0100;
const TInt KImpsServiceBLENT        = 0x0200;

// Group capability flags
const TInt KImpsServiceCREAG        = 0x0001;
const TInt KImpsServiceDELGR        = 0x0002;
const TInt KImpsServiceGETGP        = 0x0004;
const TInt KImpsServiceSETGP        = 0x0008;
const TInt KImpsServiceSUBGCN       = 0x0010;
const TInt KImpsServiceGRCHN        = 0x0020;
const TInt KImpsServiceGETGM        = 0x0040;
const TInt KImpsServiceADDGM        = 0x0080;
const TInt KImpsServiceRMVGM        = 0x0100;
const TInt KImpsServiceMBRAC        = 0x0200;
const TInt KImpsServiceREJEC        = 0x0400;


const TInt KImpsMaxSapLen       = 255; // updated from 100 to 255 for the bug fix - ESMG-72CCMU 
const TInt KImpsMaxUserIdLen    =  50;

// DATA TYPES
enum TImpsFeatures
    {
    //Features
    EFundamentalFeat = 1,
    EPresenceFeat,
    EIMFeat,
    EGroupFeat
    };

enum TImpsFunctions
    {
    //Functions
    EServiceFunc = KImpsServiceFunc,
    ESearchFunc = KImpsSearchFunc,
    EInviteFunc = KImpsInviteFunc ,
    EContListFunc = KImpsContListFunc, 
    EPresenceAuthFunc = KImpsPresAuthFunc,
    EPresenceDeliverFunc = KImpsPresDeliverFunc,
    EAttrListFunc = KImpsAttListFunc,
    EIMSendFunc = KImpsIMSendFunc,
    EIMReceiveFunc = KImpsIMReceiveFunc,
    EIMAuthFunc = KImpsIMAuthFunc,
    EGroupMgmtFunc = KImpsGroupMgmtFunc,
    EGroupUseFunc = KImpsGroupUseFunc,
    EGroupAuthFunc = KImpsGroupAuthFunc,
    EAllFuncs = KAllFuncs
    };

// General Feature class
class TImpsFeature
    {
    public:

        /**
        * Feature type accessor
        * @return feature
        */
        inline TImpsFeatures Feature( ) const;

        /**
        * Feature type mutator
        * @return aFeature feature
        */
        void SetFeature( TImpsFeatures aFeature );

        /**
        * Is this element active
        * @return True if this element exists
        */
        inline TBool IsSupported( ) const;

        /**
        * Feature activity mutator
        * @param aVal True if this element exists
        */
        inline void SetSupported( TBool aVal );

        /**
        * Function accessor
        * @param aFunc function 
        * @return EImpsTrue if the function exists
        */
        inline TBool Function( TImpsFunctions aFunc );

        /**
        * Function mutator
        * @param aFunc function 
        * @param aVal new value for the function
        */
        inline void SetFunction( TImpsFunctions aFunc, TBool aVal );

        /**
        * Capability (sub-function) accessor
        * @param aFunc sub-function(s) 
        * @return True if this element(s) exists
        */
        inline TBool Capability( TInt aFunc );

        /**
        * Capability (sub-function) mutator.
        * You should set corresponding Function with SetFunction
        * @param aCapa sub-function(s) 
        * @param aVal sub-function value 
        */
        inline void SetCapability( TInt aCapa, TBool aVal );

        /**
        * Reset
        * Functions and capabilities are cleaned expect if aAll is true.
        * @param aAll if true then all functions and capabilities are set on.
        */
        IMPORT_C void Reset( TBool aAll = EFalse );

        /**
        * Copy data
        */
        IMPORT_C void Copy( TImpsFeature& aCopy );

    private:

        /**
        * general bit map flag accessor
        * @param aValues service values
        * @prama aFlag single service flag
        * @return ETrue if the flag is set on
        */
        inline TBool IsThisFlagSet( TInt aValues, TInt aFlag ) const;

        /**
        * General bit map flag mutator
        * @param aValues service values
        * @parama aFlag single service flag
        * @param aValue new value for aFlag in aValue
        */
        inline void SetThisFlag( 
            TInt &aValues, 
            TInt aFlag,
            TBool aValue);

    private:    // Data

        TImpsFeatures   iFeature;
        TBool           iOn;
        TInt            iFunctions;
        TInt            iCapabilities;
    };

/**
*  WV Services tree data structure 
*/
class TImpsServices 
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TImpsServices();

        
    public: // New functions

        
        /**
        * Reset
        * Clean everyting expect if aAll is true.
        * @param aAll if true then all features, 
        * functions and capabilities are set on.
        */
        IMPORT_C void Reset( TBool aAll = EFalse );

        /**
        * Copy data
        */
        IMPORT_C void Copy( TImpsServices& aCopy );
      
        /**
        * Feature accessor
        */
        IMPORT_C TImpsFeature* Feature( TImpsFeatures aFeature );

    private:    // Data

        TImpsFeature            iFundamental;
        TImpsFeature            iPresence;
        TImpsFeature            iIM;
        TImpsFeature            iGroup;

    };

/**
* general bit map flag accessor
* @param aValues service values
* @prama aFlag single service flag
* @return ETrue if the flag is set on
*/
inline TBool TImpsFeature::IsThisFlagSet( TInt aValues, TInt aFlag ) const
    {
    return ( aValues & aFlag ? ETrue : EFalse );
    }

/**
* General bit map flag mutator
* @param aValues service values
* @parama aFlag single service flag
* @param aValue new value for aFlag in aValue
*/
inline void TImpsFeature::SetThisFlag( 
    TInt &aValues, 
    TInt aFlag,
    TBool aValue)
    {
    aValues = ( aValues & ~aFlag ) | ( aValue ? aFlag : 0x0000 );
    }

/**
* Feature type accessor
* @return feature
*/
inline TImpsFeatures TImpsFeature::Feature( ) const
    {
    return iFeature;
    }

/**
* Is this element active
* @return True if this element exists
*/
inline TBool TImpsFeature::IsSupported( ) const
    {
    return iOn;
    }

/**
* Feature activity mutator
* @param aVal True if this element exists
*/
inline void TImpsFeature::SetSupported( TBool aVal )
    {
    iOn = aVal;
    }

/**
* Function mutator
* @param aFunc function 
* @param aVal new value for the function
*/
inline void TImpsFeature::SetFunction( TImpsFunctions aFunc, TBool aVal )
    {
    SetThisFlag( iFunctions, aFunc, aVal );
    }

/**
* Function accessor
* @param aFunc function 
* @return EImpsTrue if the function exists
*/
inline TBool TImpsFeature::Function( TImpsFunctions aFunc )
    {
    return IsThisFlagSet( iFunctions, aFunc );
    }

/**
* Capability (sub-function) mutator.
* You should set corresponding Function with SetFunction
* @param aCapa sub-function(s) 
* @param aVal sub-function value 
*/
inline void TImpsFeature::SetCapability( TInt aCapa, TBool aVal )
    {
    SetThisFlag( iCapabilities, aCapa, aVal );
    }

/**
* Capability (sub-function) accessor
* @param aFunc sub-function(s) 
* @return True if this element(s) exists
*/
inline TBool TImpsFeature::Capability( TInt aCapa )
    {
    return IsThisFlagSet( iCapabilities, aCapa );
    }

/**
*  CspSession identifier
*/
class TImpsCspIdentifier 
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TImpsCspIdentifier();

        /**
        * Constructor
        */
        IMPORT_C TImpsCspIdentifier( const TDesC& aSap, 
                                     const TDesC& aUserId );

    public: // New functions
      
        /**
        * Sap accessor
        * Leaves with KErrNotFound if no value
        * @return SAP URI 
        */
        inline TPtrC Sap( );

        /**
        * UserId accessor
        * Leaves with KErrNotFound if no value
        * @return UserId 
        */
        inline TPtrC UserId( );

        /**
        * Sap mutator
        * @param aSap SAP URI, may include port number  
        */
        inline void SetSapL( const TDesC& aSap );

        /**
        * UserId mutator
        * @param aUserId UserId, (max 50 characters)  
        */
        inline void SetUserIdL( const TDesC& aUserId );

    private:    // Data

        TBuf<KImpsMaxSapLen>            iSAP;
        TBuf<KImpsMaxUserIdLen>         iUserId;
    };

/**
* Sap accessor
* Leaves with KErrNotFound if no value
* @return SAP URI 
*/
inline TPtrC TImpsCspIdentifier::Sap( )
    {
    return iSAP;
    }
    
/**
* UserId accessor
* Leaves with KErrNotFound if no value
* @return UserId 
*/
inline TPtrC TImpsCspIdentifier::UserId( )
    {
    return iUserId;
    }
    
/**
* Sap mutator
* @param aSap SAP URI, may include port number  
*/
inline void TImpsCspIdentifier::SetSapL( const TDesC& aSap )
    {
    iSAP = aSap;
    }
    
/**
* UserId mutator
* @param aUserId UserId, (max 50 characters)  
*/
inline void TImpsCspIdentifier::SetUserIdL( const TDesC& aUserId )
    {
    iUserId = aUserId;
    }
    
#endif       
            
