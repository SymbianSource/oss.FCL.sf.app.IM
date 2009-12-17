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
* Description:  Interface to SAP settings store.
*
*/

#ifndef __CIMPSSAPSETTINGSSTORE_H
#define __CIMPSSAPSETTINGSSTORE_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include <impssapsettings.h>

// FORWARD DECLARATIONS
class CIMPSSAPSettings;
class CIMPSSAPSettingsList;
class MIMPSSAPObserver;
class CIMPSSAPCenRep;


// CLASS DECLARATION
/**
 * Interface to SAP settings store.
 *
 * SAP settings store handles SAP Settings(CIMPSSAPSettings)
 *
 * Following services are offered:
 * - Count of stored settings
 * - Getting list of stored settings
 * - Storing new settings
 * - Retrieving settings
 * - Removing settings
 * - Set certain SAP as "default SAP" and retrieve default SAP
 *
 * SAP Settings Store offers also
 * - observer interface to listen store events
 * - transaction mechanism for single client operations
 *   this means that store, update and delete operations
 *   are guaranteed not to corrupt any data in case or errors
 *
 * @lib WVSAPSettingsStore.dll
 * @since 3.0
 */

class CIMPSSAPSettingsStore : public CBase
    {

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * Constructs concrete CIMPSSAPSettingsStore.
         *
         * Constructs and initializes the new empty storage database,
         * If requested storage database doesn't exist, creates and
         * initializes new database. If old storage database exists,
         * opens it.
         *
         * If old database to open is corrupted such way that it can't
         * be recovered, initializes a new database.
         *
         * If initializing the new database runs out of
         * disk, leaves with KErrDiskFull.
         *
         * @param aPriority Priority for active object based store event
         *        notifier. This determines priority of notifying store
         *        event observers.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettingsStore * NewL( TInt aPriority = CActive::EPriorityStandard );


        /**
         * Two-phased constructor.
         * Constructs concrete CIMPSSAPSettingsStore.
         *
         * Similar as NewL() except puts created CIMPSSAPSettingsStore object
         * to cleanupstack.
         *
         * @param aPriority Priority for active object based store event
         *        notifier. This determines priority of notifying store
         *        event observers.
         *
         * @since 3.0
         */
        IMPORT_C static CIMPSSAPSettingsStore * NewLC( TInt aPriority = CActive::EPriorityStandard );


        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIMPSSAPSettingsStore();

        //------------------------------------------------------------------
    public: // Observer support

        /**
         * Adds an observer to the store observer list. Observers
         * are notified from SAP settings store events.
         *
         * Valid access groups for this method are EIMPSIMAccessGroup and
         * EIMPSPECAccessGroup.
         *
         * Doesn't take ownership of an observer.
         *
         * @see MIMPSSAPObserver
         * @param aObserver Observer to be notified.
         * @param aGroup Group of SAPs to be observed.
         *
         * events.
         *
         * @since 3.0
         */
        IMPORT_C void AddObserverL( MIMPSSAPObserver* aObserver, TIMPSAccessGroup aGroup );

        /**
         * Removes an observer from observer list.
         *
         * If given observer isn't found, silently ignores the error.
         * @param aObserver The observer to remove.
         *
         * @since 3.0
         */
        IMPORT_C void RemoveObserver( MIMPSSAPObserver* aObserver );

        //------------------------------------------------------------------
    public: // SAP Settings


        /**
         * Gets count of currently stored SAPs in store.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *
         * Valid access groups for this method are EIMPSIMAccessGroup,
         * EIMPSPECAccessGroup and EIMPSAccessFilterAll.
         *
         * @param aGroup Access group whose SAPs are counted.
         * @return Number of SAPs in access group.
         *
         * @since 3.0
         */
        IMPORT_C TInt SAPCountL( TIMPSAccessGroup aGroup );


        /**
         * Fills CIMPSSAPSettingsList with stored SAP settings.
         *
         * Before filling, clears given array from existing list item (frees
         * also their resource reservations). After filling compacts the array.
         *
         * Clients requiring information from all stored SAPs can do it by
         * requesting first settings list with this method and then
         * iterating trough this list and loading individually required
         * SAP settings with GetSAPL() calls.
         *
         * Valid access groups for this method are EIMPSIMAccessGroup,
         * EIMPSPECAccessGroup and EIMPSAccessFilterAll.
         *
         * @param aList CIMPSSAPSettingsList to fill with SAPs.
         * @param aGroup The access group of SAPs that are populated.
         * @param aAscending Sorting order for the list. List items are sorted
         * according their names in desired order. List can contain duplicate
         * names.
         *
         * @since 3.0
         */
        IMPORT_C void PopulateSAPSettingsListL( CIMPSSAPSettingsList& aList,
                                                TIMPSAccessGroup aGroup,
                                                TBool aAscending = ETrue );

        /**
         * Gets the specified SAPs data.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - requested SAP isn't found from store
         *
         * @param aUid Uid of the SAP to get.
         * @param aSAPSettings On return will contain requested SAP's settings.
         *
         * @since 3.0
         */
        IMPORT_C void GetSAPL( TUint32 aUid, CIMPSSAPSettings* aSAPSettings );


        /**
         * Stores the new SAP's data.
         *
         * In succesful storing returns a unique uid for stored SAP and sets
         * also same uid inside of given aSAPSettings. If there isn't any
         * previous SAP's stored to store, sets this currently stored SAP
         * as default SAP. If SAP's name is not defined, leaves with KErrArgument.
         * If there exists already a SAP with same SAP name in the store,
         * leaves with KErrAlreadyExists.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - Insufficient disc space
         *   - SAP name is not defined (KErrArgument)
         *   - SAP with same SAP name exists (KErrAlreadyExists)
         *
         * Valid access groups for this method are EIMPSIMAccessGroup and
         * EIMPSPECAccessGroup.
         *
         * @param aSAPSettings The SAP settings to store.
         * @param aGroup Access group of SAP to store
         * @return Unique uid for stored SAP
         *
         * @since 3.0
         */
        IMPORT_C TUint32 StoreNewSAPL( CIMPSSAPSettings* aSAPSettings, TIMPSAccessGroup aGroup );

        /**
         * Updates the existing SAP's data.
         *
         * Stores given data to given uid. Given aUid can be different
         * than one stored inside of aSAPSettings. In this case updates
         * aSAPSettings uid to aUid.  If there exists already a SAP with
         * same SAP name in the store, leaves with KErrAlreadyExists.
         * If the old SAP is protected against updating, leaves with
         * KErrAccessDenied if SAP's name or address are being modified.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - Insufficient disc space
         *   - SAP with given uid isn't found
         *   - SAP with same SAP name exists (KErrAlreadyExists)
         *	 - SAP is protected and SAP name or address are being modified
         *
         * @param aSAPSettings The SAP settings to store.
         * @param aUid Uid of the SAP settings to update.
         *
         * @since 3.0
         */
        IMPORT_C void UpdateOldSAPL( CIMPSSAPSettings* aSAPSettings, TUint32 aUid );


        /**
         * Deletes the existing SAP.
         * If the SAP to be deleted is defined as default SAP, the default SAP id
         * set to KIMPSSettingsNullUid.
         * If the SAP to be deleted is protected against deletion,
         * leaves with KErrAccessDenied.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - SAP with given uid isn't found
         *   - SAP's protection level doesn't allow deleting
         *
         * @param aUid Uid of the SAP to delete.
         *
         * @since 3.0
         */
        IMPORT_C void DeleteSAPL( TUint32 aUid );




        //------------------------------------------------------------------
    public: // Default SAP


        /**
         * Gets the default SAP's uid.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *
         * Valid access groups for this method are EIMPSIMAccessGroup and
         * EIMPSPECAccessGroup.
         *
         * @param aUid On return will contain the uid of the default SAP.
         *		  	   aUid is set to KIMPSSettingsNullUid if
         *			   default SAP's uid isn't defined
         * @param aGroup SAP access group
         *
         * @since 3.0
         */
        IMPORT_C void GetDefaultL( TUint32& aUid, TIMPSAccessGroup aGroup );

        /**
         * Gets the default SAP's data.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - default SAP's uid isn't defined
         *   - requested (default) SAP isn't found from store
         *
         * Valid access groups for this method are EIMPSIMAccessGroup and
         * EIMPSPECAccessGroup.
         *
         * @param aSAPSettings On return will contain default SAP's settings.
         * @param aGroup SAP access group
         * @since 3.0
         */
        IMPORT_C void GetDefaultL( CIMPSSAPSettings* aSAPSettings, TIMPSAccessGroup aGroup );

        /**
         * Sets the default SAP.
         *
         * In case of error, leaves with system error codes
         * or error codes coming from underlying central repository implementation.
         * Error situations:
         *   - Insufficient memory
         *   - Insufficient disc space
         *   - SAP with given UID isn't found
         *
         * Valid access groups for this method are EIMPSIMAccessGroup and
         * EIMPSPECAccessGroup.
         *
         * @param aUid The uid of the SAP, to be set as default.
         * @param aGroup SAP access group
         * @since 3.0
         */
        IMPORT_C void SetToDefaultL( TUint32 aUid, TIMPSAccessGroup aGroup );

    private: // data

        CIMPSSAPCenRep* iImp; ///< OWN: Implementation

    };

#endif      // __CIMPSSAPSETTINGSSTORE_H

//  End of File

