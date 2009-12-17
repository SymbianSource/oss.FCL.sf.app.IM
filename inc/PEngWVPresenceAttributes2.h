/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for WV Presence Attributes
*
*/

#ifndef __PENGWVPRESENCEATTRIBUTES2_H
#define __PENGWVPRESENCEATTRIBUTES2_H

//  INCLUDES
#include <E32std.h>


//-----------------------------------------------------------------------
// SUPPORTED WV PRESENCE ATTRIBUTES


/**
 * Online status attribute.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrOnlineStatus = 0x10000001;


/**
 * Client info attribute.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrClientInfo = 0x10000002;


/**
 * Communication capabilities attribute.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrCommCap = 0x10000003;


/**
 * User availability attribute.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrUserAvailability = 0x10000004;


/**
 * Status text attribute.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrStatusText = 0x10000005;


/**
 * StatusContent attribute.
 *
 * (Former KUidPrAttrMyLogoStatusContent)
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrStatusContent = 0x10000006;


/**
 * MyLogo thumbnail.
 *
 * Supported in protocols:
 *  - WV CSP 1.1
 *  - WV CSP 1.2
 */
const TUint32 KUidPrAttrMyLogoThumbnail = 0x10000007;


/**
 * Infolink attribute.
 *
 * Supported in protocols:
 *   - WV CSP 1.2
 *
 * Infolink attribute support is variated
 * InfoLink support can be checked from:
 *  1) CenRep. See PresenceEngineInternalCRKeys.h and
 *     KPEngVariationInfoLinkSupported key. Remeber
 *     to check also protocol version from
 *     WV Engine CenRep variation file.
 *
 *  2) Attribute store. See
 *     CPEngAttributeStore2::AttributeTypeSupported()
 *     Attribute store AttributeTypeSupported() method
 *     combines both WV Engine protocol version and
 *     InfoLink variation checks.
 */
const TUint32 KUidPrAttrInfoLink = 0x10000008;


/**
 * Alias attribute.
 * Supported in protocols: WV CSP 1.1 and WV CSP 1.2
 */
const TUint32 KUidPrAttrAlias = 0x10000009;









//-----------------------------------------------------------------------
// ATTRIBUTE DATA DEFINITIONS



/**
 * Online Status.
 * (Attribute type KUidPrAttrOnlineStatus.)
 *
 * Online status indicates is the subscribed client
 * logged in to the Wireless Village server or not.
 *
 *
 * Integer data fields:
 *  - EPEngOnlineStatus
 *    Online status. Possible values enumerated
 *    in TPEngWVOnlineStatusValues. Readonly value.
 *
 * @since 3.0
 */
// Integer fields
enum TPEngWVOnlineStatusIntegerFields
    {
    EPEngOnlineStatus = 0
    };

// Enum values for EPEngOnlineStatus field
enum TPEngWVOnlineStatusValues
    {
    EPEngOnlineStatusOffline = 0, //default value
    EPEngOnlineStatusOnline = 1
    };






/**
 * Client info.
 * (Attribute type KUidPrAttrClientInfo.)
 *
 * Client info indicates to the subscriber information
 * about this particular client. This information
 * includes manufacturer, model, a user defined name,
 * the current language and a description of the client.
 * One enumerated field and five descriptive text fields.
 *
 *
 * NOTE!! This is a readonly attribute.
 * Client application can't set local values.
 *
 * Integer data fields:
 *  - EPEngCliInfDeviceType.
 *    Device type. Different values enumerated
 *    in TPEngWVCliInfDeviceTypeValues. Readonly value.
 *
 * Unicode data fields:
 *  -  EPEngCliInfDeviceManufacturer.
 *     Name of the device manufacturer. Free text, readonly.
 *
 *  -  EPEngCliInfClientProducer.
 *     Name of the client application producer. Free text, readonly.
 *
 *  -  EPEngCliInfDeviceModel.
 *     Model of the client device. Free text, readonly.
 *
 *  -  EPEngCliInfClientVersion.
 *     Version of the client application. Free text, readonly.
 *
 *  -  EPEngCliInfClientLanguage.
 *     Language setting of the client device.
 *     ISO 639-2/T (three letter language code, or empty descriptor), readonly.
 *     (Only language code length is validated.)
 *
 * @since 3.0
 */
// DesC16 fields
enum TPEngWVClientInfoDesC16Fields
    {
    EPEngCliInfDeviceManufacturer = 0,
    EPEngCliInfClientProducer = 1,
    EPEngCliInfDeviceModel = 2,
    EPEngCliInfClientVersion = 3,
    EPEngCliInfClientLanguage = 4
    };

// Integer fields
enum TPEngWVClientInfoIntegerFields
    {
    EPEngCliInfDeviceType = 0
    };

// Enum values for EPEngCliInfDeviceType field
enum TPEngWVCliInfDeviceTypeValues
    {
    EPEngCliDevTypeMobilePhone = 0,
    EPEngCliDevTypeComputer = 1,
    EPEngCliDevTypePDA = 2,
    EPEngCliDevTypeCLI = 3,
    EPEngCliDevTypeOther = 4 //default value
    };






/**
 * Communication capability.
 * (Attribute type KUidPrAttrCommCap.)
 *
 * The communication capability means the capability
 * of the device hosting the client for certain type
 * of user-to-user communication.
 *
 * One integer field and two descriptive text
 * fields in five field groups.
 *
 * Integer data fields:
 *  - EPEngCommCapStatus.
 *    The status of the communication client. Values
 *    enumerated in TPEngWVCommunicationCapabilityValues.
 *
 * DesC16 data fields:
 *  - EPEngCommCapContact.
 *    Contact address of the communication client.
 *    Free text. (not supported)
 *
 *  - EPEngCommCapNote.
 *    Free text describing the communication client.
 *    (not supported)
 *
 * Field groups for integer & DesC16 data fields:
 *  - EPEngCommCapIMClient
 *  - EPEngCommCapCallClient (not supported)
 *  - EPEngCommCapSmsClient (not supported)
 *  - EPEngCommCapMmsClient (not supported)
 *  - EPEngCommCapEmailClient (not supported)
 *
 * @since 3.0
 */
//Field groups
enum TPEngWVCommunicationCapabilitiesFieldGroups
    {
    EPEngCommCapIMClient = 0 ///< IM field group
    };

// Integer fields
enum TPEngWVCommunicationCapabilitiesIntegerFields
    {
    EPEngCommCapStatus = 0
    };

// Enum values for EPEngCommCapStatus field
enum TPEngWVCommunicationCapabilityValues
    {
    EPEngCommCapStatusClosed = 0, //default value
    EPEngCommCapStatusOpen = 1
    };






/**
 * User availability.
 * (Attribute type KUidPrAttrUserAvailability.)
 *
 * User availability status indicates how willing the publisher
 * is to start telephony or messaging conversation.
 *
 * Integer data field:
 *  - EPEngUsrAvailability.
 *    User availability. Status values are
 *    enumerated in TPEngWVUserAvailabilityValues.
 *
 * @since 3.0
 */
// Integer fields
enum TPEngWVUserAvailabilityIntegerFields
    {
    EPEngUsrAvailability = 0
    };

// Enum values from EPEngUsrAvailability field
enum TPEngWVUserAvailabilityValues
    {
    EPEngUsrAvailabilityOffline = 0, //default value
    EPEngUsrAvailabilityDiscreet = 1,
    EPEngUsrAvailabilityOnline = 2
    };








/**
 * Status text.
 * (Attribute type KUidPrAttrStatusText.)
 *
 * A short free form text description of user status.
 *
 * DesC16 data fields:
 *  - EPEngStatusText.
 *    The status text. Free text with max
 *    KPEngMaxWVStatusTextLength length.
 *
 * @since 3.0
 */
enum TPEngWVStatusTextDesC16Fields
    {
    EPEngStatusText = 0
    };

const TInt KPEngMaxWVStatusTextLength = 40;








/**
 * Status Content attribute.
 * (Attribute type KUidPrAttrStatusContent.)
 *
 * Status Content presents multimedia object that
 * the user has selected as personal status information.
 *
 * Currently only logo image (MyLogo) content is supported.
 * MyLogo is JPEG, GIF 87a or GIF 89a formatted image
 * with max 60x100 pixel size and 5 kB byte size after
 * BASE64 encoding.
 *
 * If client sets too big or wrong formatted image as MyLogo,
 * it is automaticly tried to converted to meet the MyLogo
 * requirements. Conversion can fail with:
 *  - KErrPathNotFound: image file isn't found
 *  - KErrNotSupported: tried image format not supported by the platform
 *  - KErrTooBig: image byte size still too big after JPEG conversion
 *  - or some other system-wide error code.
 *
 *
 * DesC16 fields:
 *  - EPEngMakeMyLogoDirectContentFromFile
 *    Command to generate new MyLogo direct content image
 *    asynchronously from given image file. Data parameter
 *    is image file filename. This field ID can be used
 *    both with synchronous SetDataDesC16L() and asynchronous
 *    SetDataDesC16AsyncL(). With synchronous SetDataDesC16L(), internal
 *    asynchronous processing is masqueraded as modal request
 *    with CActiveSchedulerWait and processing errors are
 *    handled with leave. If empty filename is given, clears
 *    existing logo.
 *
 *  - EPEngMyLogoDirectContentSourceFileName
 *    Gets the currently set MyLogo source file name. File name
 *    is available only for user own setted MyLogo and if the set
 *    was done with EPEngMakeMyLogoDirectContentFromFile.
 *    In other cases empty buffer is returned.
 *
 *
 * DesC8 fields:
 *  - EPEngMakeMyLogoDirectContentFromBuffer
 *    Command to generate new MyLogo direct content image
 *    asynchronously from given image data. Data parameter
 *    is images full data. This field ID can be used both
 *    with synchronous SetDataDesC8L() and asynchronous
 *    SetDataDesC16AsyncL(). With synchronous SetDataDesC8L(), internal
 *    asynchronous processing is masqueraded as modal request
 *    with CActiveSchedulerWait and processing errors are
 *    handled with leave. If empty buffer is given, clears
 *    existing logo.
 *
 *  - EPEngMyLogoDirectContentBuffer
 *    Gets current MyLogo direct content image data.
 *    If no logo is set, empty buffer is returned.
 *
 *  NOTE!! Only one set request may be active at time.
 *
 * @since 3.0
 */

// DesC16 fields.
enum TPEngWVStatusContentDesC16Fields
    {
    EPEngMakeMyLogoDirectContentFromFile = 0,
    EPEngMyLogoDirectContentSourceFileName = 1
    };


// DesC8 fields.
enum TPEngWVStatusContentDesC8Fields
    {
    EPEngMakeMyLogoDirectContentFromBuffer = 1000,
    EPEngMyLogoDirectContentBuffer = 1001
    };


// Max height for my logo image.
const TInt KPEngWVMyLogoImageMaxHeight = 60;

// Max width for my logo image.
const TInt KPEngWVMyLogoImageMaxWidth = 100;

// Max byte size for my logo image.
const TInt KPEngWVMyLogoImageMaxByteSize = 3800;

// Max byte size for BASE64 transfer encoded my logo image.
const TInt KPEngWVMyLogoImageMaxEncodedByteSize = 5120;

// My Logo mime types
_LIT8( KPEngWVMyLogoMimeJpeg,   "image/jpeg" );
_LIT8( KPEngWVMyLogoMimeImageGif,    "image/gif" );








/**
 * MyLogo thumbnail attribute.
 * (Attribute type KUidPrAttrMyLogoThumbnail.)
 *
 * MyLogoThumbnail is a thumbnail image generated
 * locally on the terminal for the received MyLogo images.
 *
 * Thumbnails are generated by the platform specific
 * Presence Engine extension plugins and thus thumbnails
 * can have platform specific charasteristics like
 * heigth, width and image type. Thumbnails are generated
 * asynchronously, so their completion is notified through
 * common Presence Engine attribute notification systems.
 * Thumbnail attribute is local attribute
 * (TPEngAttributeOriginator::EPEngLocalAttribute) and
 * thus it can't be used in attribute lists nor published.
 *
 * DesC8 fields:
 *  - EPEngMyLogoThumbnailDesC8Buf0
 *  - EPEngMyLogoThumbnailDesC8Buf1
 *  - EPEngMyLogoThumbnailDesC8Buf2
 *    8 bit descriptor fields. Usage of these buffers
 *    is up to extension plugin.
 *
 *
 * DesC16 fields:
 *  - EPEngMyLogoThumbnailDesC16Buf0
 *  - EPEngMyLogoThumbnailDesC16Buf1
 *  - EPEngMyLogoThumbnailDesC16Buf2
 *    16 bit descriptor fields. Usage of these buffers
 *    is up to extension plugin.
 *
 * @since 3.0
 */
// DesC8 fields.
enum TPEngMyLogoThumbnailDesC8Fields
    {
    EPEngMyLogoThumbnailDesC8Buf0 = 0,
    EPEngMyLogoThumbnailDesC8Buf1 = 1,
    EPEngMyLogoThumbnailDesC8Buf2 = 2
    };


// DesC16 fields.
enum TPEngMyLogoThumbnailDesC16Fields
    {
    EPEngMyLogoThumbnailDesC16Buf0 = 1000,
    EPEngMyLogoThumbnailDesC16Buf1 = 1001,
    EPEngMyLogoThumbnailDesC16Buf2 = 1002
    };







/**
 * Alias attribute.
 * (Attribute type KUidPrAttrAlias.)
 *
 * Alias attribute holds the users preferred nickname.
 *
 * DesC16 fields:
 *  - EPEngAlias
 *    Preferred alias for a contact.
 *
 * @since 3.0
 */
// DesC16 fields.
enum TPEngAliasDesC16Fields
    {
    EPEngAlias = 0
    };








/**
 * Info Link.
 * (Attribute type KUidPrAttrInfoLink.)
 *
 * NOTE: InfoLink support is still under development,
 * and details provided here might get changed.
 *
 * Info Link attribute.
 *
 * DesC16 data fields:
 *  - EPEngInfoLinkLink
 *    The Info Link.
 *
 *  - EPEngInfoLinkDesc
 *    The link description.
 *
 *  - EPEngInfoLinkMimeType
 *    The link mime type.
 *
 * DesC16 data fields are holded in groups.
 * Group ID in the get and set methods is index to wanted group.
 * Indexing is zero based. Group count can be get with
 * EPEngInfoLinkOpGetGroupCount opcode.
 *
 * New group is created when in data set the group ID
 * is the current group count. Group can be deleted
 * with EPEngInfoLinkOpDeleteGroup opcode. Enumeration
 * value is the index of to be deleted group. If value
 * -1 is given, all existing groups are deleted.
 *
 * @since 3.0
 */
// DesC16 fields.
enum TPEngInfoLinkUnicodeFields
    {
    EPEngInfoLinkLink = 0,
    EPEngInfoLinkDesc = 1,
    EPEngInfoLinkMimeType = 2
    };

//Pre defined operation groups
enum TPEngInfoLinkGroups
    {
    EPEngInfoLinkOperations = -100
    };

// Integer fields (opcodes)
enum TPEngInfoLinkEnumOpCodes
    {
    EPEngInfoLinkOpGetGroupCount = 1,
    EPEngInfoLinkOpDeleteGroup = 2
    };



#endif      // __PENGWVPRESENCEATTRIBUTES2_H

//  End of File


