/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS SAP Settings Store implementation.
*
*/


#ifndef CIMPSSAPCENREP_H
#define CIMPSSAPCENREP_H

//  INCLUDES
#include <e32base.h>
#include <commdb.h>

#include "impssapsettings.h"

//Encryption & decryption passwords has been removed
//401-1808 - Ease of Instant Messaging SERVER customization


// FORWARD DECLARATIONS

class CRepository;
class CIMPSSAPSettings;
class CIMPSSAPSettingsList;
class CIMPSSAPNotifier;
class MIMPSSAPObserver;

// CLASS DECLARATION

/**
*  Implementation for CIMPSSAPSettingsStore.
*  Uses central repository for storing SAP data.
*
*  @see CIMPSSAPSettingStore
*  @lib WVSAPSettings.dll
*  @since 3.0
*/
NONSHARABLE_CLASS( CIMPSSAPCenRep ): public CBase
    {
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPriority Priority for active object based store event
    *        notifier. This determines priority of notifying store
    *        event observers.
    */
    static CIMPSSAPCenRep* NewL( TInt aPriority );
    static CIMPSSAPCenRep* NewLC( TInt aPriority );

    /**
    * Destructor.
    */
    virtual ~CIMPSSAPCenRep();


public: // Implemented methods for CIMPSSAPSettingsStore

    /**
     * @see CIMPSAPSettingsStore for description of these methods.
     *
     * @since 3.0
     */

    // Observer support

    void AddObserverL( MIMPSSAPObserver* aObserver, TIMPSAccessGroup aGroup );
    void RemoveObserver( MIMPSSAPObserver* aObserver );

    // SAP Settings related

    TUint32 StoreNewSAPL( CIMPSSAPSettings* aSAPSettings, TIMPSAccessGroup aGroup );
    void GetSAPL( TUint32 aUid, CIMPSSAPSettings* aSAPSettings );
    void DoGetSAPL( TUint32 aUid, CIMPSSAPSettings* aSAPSettings );
    TInt SAPCountL( TIMPSAccessGroup aGroup );
    void DeleteSAPL( TUint32 aUid );
    void UpdateOldSAPL( CIMPSSAPSettings* aSAPSettings, TUint32 aUid );
    void PopulateSAPSettingsListL( CIMPSSAPSettingsList& aList,
                                   TIMPSAccessGroup aGroup,
                                   TBool aAscending = ETrue );

    // Default SAP

    void GetDefaultL( TUint32& aUid, TIMPSAccessGroup aGroup );
    void GetDefaultL( CIMPSSAPSettings* aSAPSettings, TIMPSAccessGroup aGroup );
    void SetToDefaultL( TUint32 aUid, TIMPSAccessGroup aGroup );

private: //Constructors

    /**
    * C++ default constructor.
    */
    CIMPSSAPCenRep();

    /**
    * By default Symbian 2nd phase constructor is private.
    *
    * @param aPriority Priority for active object based store event
    *        notifier. This determines priority of notifying store
    *        event observers.
    */
    void ConstructL( TInt aPriority );

private: //Helper methods

    /**
     * Calculates cenrep Id for a setting using bitmasks.
     *
     * @param aSetting The enumerated setting value to use in calculation
     * @param aUid Uid of the SAP.
     * @param aGroup Access group of the SAP
     * @return The calculated Id
     * @since 3.0
     */
    TUint32 MaskedId( TInt aSetting, TUint32 aUid, TIMPSAccessGroup aGroup );

    /**
     * Checks if SAP with given name already exists.
     *
     * @param aSAPName Name of the SAP to be checked
     * @param aUid SAP's name in SAP with this Uid is allowed to be duplicate
     *		  to enable updating of existing SAPs
     * @param aGroup Access group which within the check is made
     * @return ETrue if name already exists, otherwise EFalse
     * @since 3.0
     */

    TBool SAPNameExistsL( const TDesC& aSAPName, TUint32 aUid, TIMPSAccessGroup aGroup );

    /**
     * Checks if SAP exists in the central repository. Leaves
     * wíth KErrNotFound if SAP does not exist.
     *
     * @param aUid Uid to check
     * @since 3.0
     */

    void SAPExistsL( TUint32 aUid );

    /**
     * Gets new unique uid for storing new SAP.
     *
     * @return New Uid, leaves with KErrInUse if all Uids are in use
     * @since 3.0
     */

    TUint32 NewUidL();

    /**
     * Gets SAP access group .
     *
     * @param aUid Uid of the SAP for getting access group
     * @return SAP access group
     * @since 3.0
     */

    TIMPSAccessGroup SAPTypeL( TUint32 aUid );

    /**
     * Gets SAP protection level.
     *
     * @param aUid Uid of the SAP for getting protection
     * @param aGroup SAP's access group
     * @return SAP protection level
     * @since 3.0
     */

    TIMPSSAPProtection SAPProtectionL( TUint32 aUid, TIMPSAccessGroup aGroup );

    /**
     * Sets up default SAP ids if they do not exist in the repository.
     *
     * @since 3.0
     */

    void SetUpDefaultsL();

    /**
     * Finds existing SAP ids from desired access group and populates them
     * to an array.
     *
     * @param aGroup desired access group
     * @param aFoundSAPs array for storing found SAP ids
     * @since 3.0
     */

    void FindSAPsFromAccessGroupL( TIMPSAccessGroup aGroup,
                                   RArray<TUint32>& aFoundSAPs );

    /**
     * Starts new transaction if there is no ongoing transaction.
     *
     * @param aMode Transaction mode
     * @since 3.0
     */

		TBool StartOwnTransactionL( TInt aMode );

    /**
     * Converts Access Point Name into AP value
     * Configurability req.
     * 401-1808 - Ease of Instant Messaging SERVER customization
     * @param aAPName - AP name
     * @return AP Number in the Commnection DB
     * @since 5.0
     */

    TUint32 DoGetAccessPointsL( const TDesC& aAPName );


    /**
     * Converts Access Point Number into AP Name value
     * Configurability req.
     * 401-1808 - Ease of Instant Messaging SERVER customization
     * @param aAP - AP Number in the Commnection DB
     * @return HBufC AP name, Caller should free this data.
     * @since 5.0
     */
    HBufC* DoGetAccessPointsNameL( const TUint32 aAP );

		/**
		* Reset iInTransaction state by invoke ReSetTransactionFlag(). 
		* This happens when iRepository is not committed and leave occurred.
		* @param aAP - Instance of this class
		*/		
		static void ReSetTransactionState(TAny* aPtr);

		/**
		* Reset iInTransaction flag
		*/	
		void ReSetTransactionFlag();

    /**
     * Unit test class is a friend class.
     */
    friend class T_CIMPSSAPSecurity;	// CSI: 36 #

private:    // Data
    CRepository* iRepository; // Handle to central repository session (own)
    CIMPSSAPNotifier* iNotifier; // SAP Settings notifier (own)
    CCommsDatabase* iCommsDatabase; //Accesses the communications database
    TBool iInTransaction;
    };

#endif      // CIMPSSAPCENREP_H

// End of File
