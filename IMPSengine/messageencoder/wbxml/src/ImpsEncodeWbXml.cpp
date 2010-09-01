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
* Description:  Imps engine wbxml message encoder
*
*
*/


// INCLUDE FILES
#include    <e32std.h>
#include    <apparc.h>
#include    <s32mem.h>
#include    <utf.h>
#include    <e32math.h>
#include    <miutconv.h>
//#include    <mentact.h>

#include    "ImpsWbXmlData.h"
#include    "ImpsEncodewbxml.h"
#include    "ImpsDataAccessorapi.h"
#include    "ImpsErrors.h"
#include    "Imps_1_1_Tokens.h"
#include    "Imps_1_2_Tokens.h"
#include    "ImpsWbXmlCommon.h"
#include    "ImpsCspDtd.h"
#include    "ImpsUtils.h"
#include    "ImpsServices.h"
#include    "ImpsVariantAPI.h"
#include    "impsfields.h"
#include    "ImpsDataUtils.h"

// ================= LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImpsEncodeWbXml::CImpsEncodeWbXml( ):
    iCspVersion( EImpsCspVersion11 )
    {

    }

// default constructor can leave.
void CImpsEncodeWbXml::ConstructL()
    {
  
    iElementValue = HBufC::NewL(KImpsWbXmlMaxStringLength);
    iStoreValue = HBufC8::NewL(KImpsWbXmlMaxStringLength);

    // create handler for wbxml data
    iWbXmlData = CImpsWbXmlData::NewL( );

    iCurrentDictionary = iDictionaryCount = 0;
    iDictionaries[iDictionaryCount++] = 
        (NW_WBXML_Dictionary_s*)&NW_Imps_1_1_WBXMLDictionary;
    iDictionaries[iDictionaryCount++] = 
        (NW_WBXML_Dictionary_s*)&NW_Imps_1_2_WBXMLDictionary;
    if(NW_WBXML_Dictionary_initialize (iDictionaryCount,iDictionaries) != NW_STAT_SUCCESS)
    {
        User::Leave(KImpsErrorEncode);
    }

    // PEC in use ?
    CImpsVariant* dpb = CImpsVariant::NewLC();
    if (dpb->IsFeatureSupportedL(EDpb))
    {
        iXmlUtils = CImpsXmlUtils::NewL( );
        iPEC = ETrue;
    }
    else
    {
        iXmlUtils = NULL;
        iPEC = EFalse;
    }
    
   CleanupStack::PopAndDestroy();
    
    }

// Two-phased constructor.
CImpsEncodeWbXml* CImpsEncodeWbXml::NewL( )
    {

    CImpsEncodeWbXml* self = new (ELeave) CImpsEncodeWbXml( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CImpsEncodeWbXml::~CImpsEncodeWbXml()
    {
    
    delete iWbXmlData;
    delete iElementValue;
    delete iStoreValue;
    delete iXmlUtils;
    NW_WBXML_Dictionary_destroy(); 
    
    }

// ---------------------------------------------------------
// CImpsEncodeXml::EncodeMessageL
// wbxml message encoder main function, which provides API
// class aImpsData contains message data,
// encoded message is stored to aEncodeBuffer
// ---------------------------------------------------------
//
void CImpsEncodeWbXml::EncodeMessageL(
    MImpsDataAccessor& aImpsData,
    CBufFlat& aEncodeBuffer)
    {

    // INIT BUFFERS
    iImpsData = &aImpsData;
    iEncodeBuffer = &aEncodeBuffer;
        
    // CREATE EMPTY DOM DOCUMENT TREE
    NW_TinyDom_Handle_t handle;
    
    // set up the version related things
    iCspVersion = iImpsData->GetImpsFields()->CspVersion();
    
    TInt cspVersion = 
        ((iCspVersion == EImpsCspVersion11)?KImpsWvCsp11PublicIdentifier:KImpsWvCsp12PublicIdentifier);


	iCurrentDictionary =
    	((iCspVersion == EImpsCspVersion11)? 0 : 1);	
    	
    iDocument = NW_DOM_DocumentNode_createDocumentWithNumberPublicId(&handle,
    KImpsWbXmlVersion,cspVersion,KImpsDefaultCharset,NW_TRUE,NW_TRUE);
    if(!iDocument)
    {
        User::Leave(KImpsErrorEncode);
    }

    // check for possible buffer allocation failure
    if(!iElementValue)
    {
        iElementValue = HBufC::NewL(KImpsWbXmlMaxStringLength);
    }
    if(!iStoreValue)
    {
        iStoreValue = HBufC8::NewL(KImpsWbXmlMaxStringLength);
    }

    // create key for data accessor
    iAccessKey = iImpsData->NewKeyL();
    iNameSpace = KImpsNameSpaceCspMessage;


    // start building DOM document tree
    TInt rcode = KErrNone;
    TBufC<16> root(KImpsWvCspMessage);
    TRAPD(error,rcode = EncodeElementL(root,iDocument,0))
    if(error != KErrNone || rcode != KErrNone)
    {
    
        // FAILURE !
        // delete DOM document
        NW_DOM_DocumentNode_Delete(iDocument);
        // delete access key        
        iAccessKey->Destroy( );
        if(rcode != KErrNone)
        {
            User::Leave(rcode);
        }
        else
        {
            User::Leave(KImpsErrorEncode);
        }
    }
    iAccessKey->Destroy( );

    // ENCODE MESSAGE
    TUint32 size = 0;
    unsigned char *buffer = NULL;
    NW_Encoder_t WbXmlEncoder;
    if(NW_Encoder_encodeWBXML(&WbXmlEncoder,iDocument,NW_TRUE,&size,&buffer) != NW_STAT_SUCCESS)
    {
        // FAILURE !
        // delete DOM document
        NW_DOM_DocumentNode_Delete(iDocument);
        if(buffer)
        {
            free(buffer);
        }
        User::Leave(KImpsErrorEncode);
    }

    // put the new public ID there
//    if (size >0)
//        {
//        buffer[1] = 0x10;
//        }
    
    // copy message to aEncodeBuffer
    if((TInt)size > iEncodeBuffer->Size())
    {
        iEncodeBuffer->Reset();
    }
    
    iEncodeBuffer->ResizeL(size);
    iEncodeBuffer->Write(0,buffer,size);
    free(buffer);

    // delete DOM document
    NW_DOM_DocumentNode_Delete(iDocument);

    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::ReadElementDeclaration
// method reads elements DTD declaration
// ---------------------------------------------------------
//
TPtrC CImpsEncodeWbXml::ReadElementDeclaration(TDesC &aElement)
    {

    
    TPtrC dtd;
    if ( iCspVersion == EImpsCspVersion11 ) 
        {
        dtd.Set(KCspHeaderDtd11[NULL]);
        }
    else
        {
        dtd.Set(KCspHeaderDtd12[NULL]);
        }
    
    TInt i(0);
    TInt count = 0;

    if(iNameSpace == KImpsNameSpaceTransactionContent)
        {
        count = ((iCspVersion == EImpsCspVersion11)?KImpsTransactionDtd11Count:KImpsTransactionDtd12Count);
        }
    else if(iNameSpace == KImpsNameSpacePresenceSubList)
        {
        count = ((iCspVersion == EImpsCspVersion11)?KImpsPresenceDtd11Count:KImpsPresenceDtd12Count);
        }
    else
        {
        count = ((iCspVersion == EImpsCspVersion11)?KImpsHeaderDtd11Count:KImpsHeaderDtd12Count);
        }
    for(i=0;i<count;i++)
    {
        switch(iNameSpace)
        {
         
        case KImpsNameSpaceCspMessage:
            if ( iCspVersion == EImpsCspVersion11 )
                {
                dtd.Set(KCspHeaderDtd11[i]);
                }
            else
                {
                dtd.Set(KCspHeaderDtd12[i]);
                }
            break;

        case KImpsNameSpaceTransactionContent:
            if ( iCspVersion == EImpsCspVersion11 )
                {
                dtd.Set(KCspContentDtd11[i]);
                }
            else
                {
                dtd.Set(KCspContentDtd12[i]);
                }
            break;

        case KImpsNameSpacePresenceSubList:
            if ( iCspVersion == EImpsCspVersion11 )
                {
                dtd.Set(KCspPresenceDtd11[i]);
                }
            else
                {
                dtd.Set(KCspPresenceDtd12[i]);
                }
            break;

        }
        if(dtd.Find(aElement) == 1 && dtd.Locate(' ') == (aElement.Length() + 1))
        {
            break;
        }
    }
    
    return(dtd);
    
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeElementL
// method adds one node to DOM tree
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeElementL(TDesC &aElement, NW_DOM_ElementNode_t* aNode,TInt aIndex)
    {

    TBuf<sizeof(SImpsValueList)> valuebuf(sizeof(SImpsValueList));
    SImpsValueList* valuelist = (SImpsValueList*)valuebuf.Ptr();

    // Read element declaration
    TPtrC element = ReadElementDeclaration(aElement);

    // prepare child element list
    ReadElementList(element,valuelist);

    // Get element WBXML token
    TBool extraelem = EFalse;
    TInt token = ReadToken(element);

    TInt elementcounter = 0; 
    TBool literal = false;

    if(token == KErrNotFound)
    {
    	// check for the extension token APIClient as it is not in the dictionary
        if ( (aElement.CompareF(KImpsAPIClient))==0 ) 
            {
            literal = true;
            }
        else
            {
        	extraelem = ETrue;
        	elementcounter = aIndex;
            }
    }

    // Update data accessor key (root element is left out)
    TInt primitive = KErrNone;
    if(token != KImpsMessageRootElement && !extraelem)
    {
        if ( literal ) 
            {
            // add the APIClient key to accessor key
            iAccessKey->AddL(CREATEKEY(EImpsKeyAPIClient,aIndex),EImpsKeyTypeIM);
            }
        else
            {
        	primitive = SetDataAccessorKeyL(token,aIndex);
            }
    }
							
    TInt rcode = KErrNotFound;
    NW_DOM_ElementNode_t *node = NULL;
	TInt exrcode = KErrNotFound;
        
    if(iImpsData->CheckBranchExistenceL(iAccessKey) || extraelem)
    {
        if ( literal ) 
            {
            HBufC8* literalToken = HBufC8::NewL(aElement.Length()+1);// for the \0 at the end
            CleanupStack::PushL(literalToken);
            literalToken->Des().Copy(aElement);

            // create an element node from string for extension element
            NW_String_t* elementString = NW_String_new();
            NW_String_initialize(elementString, (void*)literalToken->Des().PtrZ(), HTTP_utf_8);
            node = NW_DOM_DocumentNode_createElementNode(iDocument, elementString);
            NW_String_delete(elementString);
            
            // literalToken has to be deleted as last because NW_String keeps the pointer
            CleanupStack::PopAndDestroy(); // literalToken
            }        
        else
            {
            node = NW_DOM_DocumentNode_createElementNodeByToken(iDocument,(TUint16)token);
            }

            if(node != NULL || extraelem)
			    {
			        if((rcode = EncodeAttributes(token,node)) == KErrNone || extraelem)
			        {
			            if(!(valuelist->Flags & KImpsValueListEmpty))
			            {
			                TPtrC value = ReadNextValue(valuelist,element);
			                if(value.Find(KPcData) != KErrNotFound)
			                {
			                    // encode value
			                    rcode = EncodeValueL(token,node,aIndex);
			                }
			                else
			                {
			                    for(;;)
			                    {
			                        if(valuelist->Flags & KImpsValueListAlternative && !extraelem)
			                        {
			                            if(primitive == KErrNotFound)
			                            {
			                                TImpsKeyType type = EImpsKeyTypeIM;
			                                iImpsData->RestoreAlternativeL(iAccessKey,primitive,aIndex,type);
			                            }
			                            // Get the element name string from the 1.2 dictionary as it has all the elements
			//                                TPtrC elementName(KNullDesC);
			                            
			                            TUint16* elementNamePtr = (TUint16*)NW_WBXML_Dictionary_getTagByFqToken((NW_Uint32)(2<<16)+primitive)->bytes;
			                            if (elementNamePtr)
			                            	{
			                            	value.Set(elementNamePtr);
			                            	}
										else
											{
											value.Set(KNullDesC);
											}
			                            
			                            //value.Set(iWbXmlData->GetDtdValue(token,primitive));
			                        }
			                        if(extraelem)
			                        {
			                            node = aNode;
			                        }
			                        rcode = EncodeElementL(value,node,elementcounter);
									if(rcode == KErrNone && extraelem)
									{
										exrcode = KErrNone;
									}
									value.Set(FetchNextElement(valuelist,elementcounter,rcode,element,value));
									if(rcode == KErrNotFound)
									{
										if(extraelem) rcode = exrcode;
										else rcode = KErrNone;
										break;
									}
									if(rcode == KImpsErrorEncode || rcode == KImpsErrorValidate)
			                        {
			                            if(extraelem) 
			                                {
			                                rcode = KErrNotFound;
			                                }
										else 
			                                {
			                                rcode = KImpsErrorValidate;
			                                }
			                            break;
			                        }		
			                    }
			                }
			            }
			            if(rcode == KErrNone && !extraelem)
			            {
			                // append node to parent node
			                NW_Status_t status = NW_DOM_Node_appendChild((NW_TinyTree_Node_s*)aNode,node);
			                if(status != NW_STAT_SUCCESS) 
			                    {
			                    rcode = KImpsErrorEncode;
			                    }
			            }
			        }
			        else
			        {
			            rcode = KImpsErrorEncode;
			        }
			    }
			    else
			    {
			        rcode = KImpsErrorEncode;
            	}
    	}
    else
    {
        if(token == KImpsWbXmlContentSize)
        {
            // special case, content size must be calculated
            rcode = SetContentSizeL(token,aNode);
        }
        else
        {
            rcode = KErrNotFound;
        }
    }

    // update data access key
    if(!extraelem)
    {
        PopAccessKeyL(token);
    }

    return(rcode);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::SetContentSize
// method counts message content size
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::SetContentSizeL(TInt aToken,NW_DOM_ElementNode_t* aNode)
    {
    iAccessKey->PopL(2);
    iAccessKey->AddL(EImpsKeyContentData);
    
    TInt size(0);
    if ( TImpsDataUtils::GetContentDataTypeL( iImpsData, 0 ) == EImpsDataTypeDesc )
        {
    TDesC *p;
        if ( iImpsData->RestoreDescL(iAccessKey,p) )
            {
            size = p->Length();   
            }
        }
     else
         {
         TDesC8 *p;
         if ( iImpsData->RestoreDesc8L(iAccessKey,p) )
            {
            size = p->Length();   
            }
         }

    iAccessKey->PopL();
    iAccessKey->AddL(EImpsKeyMessageInfo);
    iAccessKey->AddL(EImpsKeyContentSize);
    iImpsData->StoreIntegerL(iAccessKey,size);

    NW_DOM_ElementNode_t *node = NULL;
    TInt rcode = KErrNone;

    if((node = NW_DOM_DocumentNode_createElementNodeByToken(iDocument,(TUint16)aToken)) != NULL)
    {
        rcode = EncodeValueL(aToken,node,0);
      
        if(rcode == KErrNone)
        {
            // append node to parent node
            NW_Status_t status = NW_DOM_Node_appendChild((NW_TinyTree_Node_s*)aNode,node);
            if(status != NW_STAT_SUCCESS)
            {
                rcode = KImpsErrorEncode;   
            }
        }
        
    }   
    else
    {
        return(KImpsErrorEncode);
    }

    return(rcode);
    }


// ---------------------------------------------------------
// CImpsEncodeWbXml::PopAccessKeyL
// method updates data accessor key
// ---------------------------------------------------------
//
void CImpsEncodeWbXml::PopAccessKeyL(TInt aToken)
    {

    if(iAccessKey->Count())
    {
        iAccessKey->PopL( );
        switch(aToken)
        {
            case KImpsWbXmlTransactionContent:
            iNameSpace = KImpsNameSpaceCspMessage;
            break;
            
            case KImpsWbXmlPresenceSubList:
            iNameSpace = KImpsNameSpaceTransactionContent;
            break;
                
            default:
            break;
        }
    }

    }
// ---------------------------------------------------------
// CImpsEncodeWbXml::FetchNextElement
// medhod fetches next child element if exists
// ---------------------------------------------------------
//
TPtrC CImpsEncodeWbXml::FetchNextElement(SImpsValueList* aValueList,TInt& aElementCounter,
                                         TInt& aRcode, TPtrC aElementList, TPtrC aCurrentElement)
    {

    TPtrC value;

    if(aRcode == KErrNone)
    {
        if(aValueList->Flags & KImpsValueListMultiple ||
           aValueList->Flags & KImpsValueListOnceOrMore)
        {
            value.Set(aCurrentElement);
            aElementCounter++;
            aRcode = KErrNone;
        }
        else
        {
            if(!(aValueList->Flags & KImpsValueListEnd) && !(aValueList->Flags & KImpsValueListAlternative))
            {
                value.Set(ReadNextValue(aValueList,aElementList));
                aElementCounter = 0;
                aRcode = KErrNone;
            }
            else
            {
                aRcode = KErrNotFound;
            }
        }
    }
    else
    {
        if(aRcode == KErrNotFound)
        {
            if(aValueList->Flags & KImpsValueListMultiple ||
               aValueList->Flags & KImpsValueListOptional ||
               aValueList->Flags & KImpsValueListAlternative)               
            {
                if((aValueList->Flags & KImpsValueListAlternative) && aElementCounter)
                {
                    aRcode = KErrNotFound;
                }
                else if(!(aValueList->Flags & KImpsValueListEnd))
                {
                    value.Set(ReadNextValue(aValueList,aElementList));
                    aElementCounter = 0;    
                    aRcode = KErrNone;
                }
                else
                {
                    if(aValueList->Flags & KImpsValueListAlternative)
                    {
                        aRcode = KImpsErrorEncode;
                    }
                    else
                    {
                        aRcode = KErrNotFound;
                    }
                }
            }
            else if(aValueList->Flags & KImpsValueListOnceOrMore && aElementCounter)
            {
                if(!(aValueList->Flags & KImpsValueListEnd))
                {
                    if(!(aValueList->Flags & KImpsValueListEnd))
                    {
                        value.Set(ReadNextValue(aValueList,aElementList));
                        aElementCounter = 0;          
                        aRcode = KErrNone;
                    }
                    else
                    {
                        aRcode = KErrNotFound;
                    }
                }
            }             
            else
            {
                aRcode = KImpsErrorEncode;
            }
        }          
    }
    
    return value;

    }
// ---------------------------------------------------------
// CImpsEncodeWbXml::SetDataAccessorKeyL
// method updates data accessor key
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::SetDataAccessorKeyL(TInt aToken, TInt aIndex)
    {

    TInt impskey = KErrNotFound;
    TInt primitive = KErrNotFound;

	impskey = aToken;
    switch(iNameSpace)
    {
    
    case KImpsNameSpaceCspMessage:
    
        iKeyType = EImpsKeyTypeIM;
//        impskey = iWbXmlData->GetHeaderAccessKey(aToken);
        iAccessKey->AddL(CREATEKEY(impskey,aIndex),iKeyType);
        break;
        
    case KImpsNameSpaceTransactionContent:
        iKeyType = EImpsKeyTypeIM;
//        impskey = iWbXmlData->GetContentAccessKey(aToken);
        iAccessKey->AddL(CREATEKEY(impskey,aIndex),iKeyType);
        break;
    
    case KImpsNameSpacePresenceSubList:
        iKeyType = EImpsKeyTypePre;
//        impskey = iWbXmlData->GetPresenceAccessKey(aToken);
                // impskey needs to be changed because 
                // the ContentType is in another namespace
                if ( impskey == EImpsKeyContentType)
                    {
                    iKeyType = EImpsKeyTypeIM;
                    }
        iAccessKey->AddL(CREATEKEY(impskey,aIndex),iKeyType);
        break;

    default:
        break;
    
    }

    if(aToken == KImpsWbXmlTransactionContent)
    {
        iNameSpace = KImpsNameSpaceTransactionContent;

        if(iPEC)
        {
            // pure data handle
            primitive = iXmlUtils->XmlToTransactionContentL(*iImpsData);
        }
    }

    if(aToken == KImpsWbXmlPresenceSubList)
    {
        iNameSpace = KImpsNameSpacePresenceSubList;
    }
    
    return(primitive);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::ReadElementList
// method inits child element list
// ---------------------------------------------------------
//
void CImpsEncodeWbXml::ReadElementList(TPtrC aElement, SImpsValueList* aElementList)
    {

    aElementList->Begin = aElement.Locate('(');
    if(aElementList->Begin == KErrNotFound) 
    {
        aElementList->Flags |= KImpsValueListEmpty;
    }
    else
    {
        aElementList->Length  = aElement.Length() - aElementList->Begin;
        aElementList->Flags = 0;
        aElementList->Next = aElementList->Begin + 1;
    }

    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::ReadToken
// method reads WBXML token corresponding to element name
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::ReadToken(TPtrC aElement)
    {
        
    TInt length = aElement.Locate(' ')-1;
    iElementValue->Des().Zero();
    for(TInt i=0;i<length;i++)
    {
        iElementValue->Des().Append(aElement[i+1]);
    }
    iElementValue->Des().Append(NULL);

    return(NW_WBXML_Dictionary_getTagToken(iDictionaries[iCurrentDictionary],(NW_String_UCS2Buff_t*)iElementValue->Ptr(),NW_TRUE));

    }
// ---------------------------------------------------------
// CImpsEncodeWbXml::StoreInteger
// method stores integer value
// ---------------------------------------------------------
//
void CImpsEncodeWbXml::StoreInteger(TUint32 aValue, TPtr8 aBuffer)
    {
    
    TInt i = 0;
    for(i=3;i>0;i--)
    {
        if((aValue >> (8*i)) & 0xff) break;
    }
    for(;i>0;i--)
    {
        aBuffer.Append((TUint8)(aValue >> (8*i)));
    }
    
    aBuffer.Append((TUint8)aValue);
    }
// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeAttributes
// method adds attribute values to (one) DOM node if exists
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeAttributes(TInt aToken, NW_DOM_ElementNode_t* aNode)
    {
    
    TUint value = iWbXmlData->GetWbXmlAttributeTokenValue(aToken, iCspVersion);
    if(value)
    {
        TPtrC8 str = iWbXmlData->GetWbXmlAttributeStringValue(aToken, iCspVersion);
        if(str.Length())
        {
            NW_String_initialize(&iString,(TUint8*)str.Ptr(),HTTP_utf_8);
            if(NW_DOM_AttrVal_initFromString(&iAttribute,&iString) != NW_STAT_SUCCESS)
            {
             
                return KImpsErrorEncode;
            }
        }
        if(NW_DOM_ElementNode_setAttributeByToken(aNode,(NW_Uint16)value,&iAttribute) != NW_STAT_SUCCESS)
        {
            return KImpsErrorEncode;
        }
    }
    else
    	{
    	// ExtBlock handling is a special case
    	if (aToken == EImpsKeyExtBlock)
    		{
	    	NW_String_initialize(&iString,(TUint8*)KAPIClientAttributeValue().Ptr(),HTTP_utf_8);
            if(NW_DOM_AttrVal_initFromString(&iAttribute,&iString) != NW_STAT_SUCCESS)
	            {
             
                return KImpsErrorEncode;
    	        }
	    	
	    	NW_String_initialize(&iString,(TUint8*)KAPIClientAttribute().Ptr(),HTTP_utf_8);
    		if (NW_DOM_ElementNode_setAttributeByAttrVal(aNode, &iString, &iAttribute )!= NW_STAT_SUCCESS)
	            {
                return KImpsErrorEncode;
	            }
    		}
    	}
    
    return(KErrNone);
    }
// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeValueL
// method reads element value from data accessor and stores
// it to DOM tree
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeValueL(TInt aToken, NW_DOM_TextNode_t *aNode,TInt aIndex)
    {
    // this removes compiler warning
    aIndex = KErrNone;
    TInt    rcode       = aIndex;
    TInt    elemcount   = 0;

    switch(iImpsData->KeyTypeL(iAccessKey))
    {
    case EImpsDataTypeContentData:
        { 
         if ( TImpsDataUtils::GetContentDataTypeL( iImpsData, 0 ) == EImpsDataTypeDesc8 )
            {
            rcode = EncodeString8L(aToken,elemcount);
            }
         else
            {
            rcode = EncodeStringL(aToken,elemcount);
            }
        }
        break;
    case EImpsDataTypeDesc8:
        rcode = EncodeString8L(aToken,elemcount);
        break;

    case EImpsDataTypeDesc:
        rcode = EncodeStringL(aToken,elemcount);
        break;

    case EImpsDataTypeInt:
        rcode = EncodeIntegerL(aToken,elemcount);
        break;

    case EImpsDataTypeBoolean:
        rcode = EncodeBooleanL(aToken,elemcount);
        break;

    case EImpsDataTypeExt:
        rcode = EncodeExtL(aToken,elemcount);
        break;

    default:
        rcode = KErrNotFound;
    }

    if(rcode == KErrNone)
    {
        for(TInt i=0;i<elemcount;i++)
        {
            NW_DOM_TextNode_t *node = NW_DOM_DocumentNode_createTextNodeWithTextItem(iDocument,&iTextItem[i]);
            if(node)
            {
            if(NW_DOM_Node_appendChild((NW_TinyTree_Node_s*)aNode,node) != NW_STAT_SUCCESS)
                {
                rcode = KImpsErrorEncode;
                break;
                }
            }
        }
    }
    if(iStoreValue->Des().Length() > KImpsWbXmlMaxBufferSize)
    {
        delete iStoreValue;
        iStoreValue = NULL;
        iStoreValue = HBufC8::NewL(KImpsWbXmlMaxStringLength);
    }
    
    return(rcode);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeStringL
// method stores string value to DOM node
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeStringL(TInt aToken,TInt& aElemCount)
    {

    NW_Status_t status = NW_STAT_SUCCESS;
    TInt rcode = KErrNone;
    TInt elem = 0;
    TInt value = 0;
    TInt maxlength = 0;

    TDesC *p;
    if(iImpsData->RestoreDescL(iAccessKey,p))
    {
        // presence data handling
        if(iNameSpace == KImpsNameSpacePresenceSubList)
        {
            TInt token = iAccessKey->GetElementL((iAccessKey->Count() - 2),iKeyType);

//            // Lookup the value string in dictionary: NW_FALSE = no case sensitive
//            if((elem = NW_WBXML_Dictionary_getAttributeToken(
//                &NW_Imps_1_2_WBXMLDictionary, (NW_String_UCS2Buff_t*)p->Ptr(), 0, NW_FALSE)) != KErrNotFound)
            if((elem = iWbXmlData->GetWbXmlElementValue(token,p)) != KImpsWbXmlDataNotFound)
            {
                status = NW_DOM_TextItem_initFromExtensionInt(&iTextItem[aElemCount++],NW_WBXML_EXT_T_0,elem);
                if(status != NW_STAT_SUCCESS)
                {
                    rcode = KImpsErrorEncode;
                }
                return rcode; // do not continue as we already added the string as extension
            }
        }

//        if((elem = NW_WBXML_Dictionary_getAttributeToken(
//            &NW_Imps_1_2_WBXMLDictionary, (NW_String_UCS2Buff_t*)p->Ptr(), 0, NW_FALSE)) != KErrNotFound)
        if((elem = iWbXmlData->GetWbXmlElementValue(aToken,p)) != KImpsWbXmlDataNotFound)
        {
            status = NW_DOM_TextItem_initFromExtensionInt(&iTextItem[aElemCount++],NW_WBXML_EXT_T_0,elem);
            if(status != NW_STAT_SUCCESS)
            {
                rcode = KImpsErrorEncode;
            }
            else
            {
                rcode = KErrNone;
            }
            return(rcode);
        }
        iWbXmlData->GetWbXmlElementValue(aToken,value,maxlength);
		TPtr8 p8 = iStoreValue->Des();
		TInt size = CountContentSize(p);
		if(size >= p8.MaxLength())
		{
			delete iStoreValue;
			iStoreValue = NULL;
			iStoreValue = HBufC8::NewL(size+1);
			p8.Set(iStoreValue->Des( ));
		}
		if(CnvUtfConverter::ConvertFromUnicodeToUtf8(p8,*p) == KErrNone)
		{
			// if its content data, only then escape XML which will convert
			// special characters like & to &amp;.
			if (0x0D == aToken)
			{
			// Fix TSW Error Id: EDZG-7KXCAF 
			iStoreValue = iXmlUtils->EscapeXmlL(p8).AllocL();
			iStoreValue = iStoreValue->ReAllocL(iStoreValue->Length() + 1 );
			}	
					
			iStoreValue->Des().Append(0);
			TInt offset = 0;
			elem = SearchPrefix(iStoreValue->Des(),offset);
			if(elem != KErrNotFound)
			{
				status = NW_DOM_TextItem_initFromExtensionInt(&iTextItem[aElemCount++],NW_WBXML_EXT_T_0,elem);
				if(status != NW_STAT_SUCCESS)
				{
					return(KImpsErrorEncode);
				}
			}

			NW_String_initialize(&iString,(TUint8*)iStoreValue->Des().Mid(offset).Ptr(),HTTP_utf_8);
			status = NW_DOM_TextItem_initFromString(&iTextItem[aElemCount++],&iString);
			if(status != NW_STAT_SUCCESS)
			{
				rcode = KImpsErrorEncode;
			}
		}
		else
		{
			rcode = KImpsErrorEncode;
		}
    }
    else
    {
        rcode = KErrNotFound;
    }

    return(rcode);
}

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeString8L
// method stores 8 bit string value to DOM node
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeString8L(TInt aToken,TInt& aElemCount)
    {
    
    NW_Status_t status = NW_STAT_SUCCESS;
    TInt rcode = KErrNone;
    TInt elem = 0;
    TInt value = 0;
    TInt maxlength;

    TDesC8 *p8;
    if(iImpsData->RestoreDesc8L(iAccessKey,p8))
    {
        if((elem = iWbXmlData->GetWbXmlElementValue(aToken,value,maxlength))
                 != KImpsWbXmlDataNotFound)
        {
            StoreInteger(elem,iStoreValue->Des());
            iString.storage = (TUint8*)iStoreValue->Ptr();
            iString.length = iStoreValue->Length();
        }
		if(p8->Length() >= iStoreValue->Des().MaxLength())
		{
			delete iStoreValue;
			iStoreValue = NULL;
			iStoreValue = HBufC8::NewL(p8->Length() + 1);
		}
		iStoreValue->Des().Copy(*p8);
/*
		if(aToken == KImpsWbXmlDirectContent || aToken == KImpsWbXmlContainedvCard)
		{
			iOpaque.data = (TUint8*)iStoreValue->Ptr();
			iOpaque.length = iStoreValue->Length();
			status = NW_DOM_TextItem_initFromOpaque(&iTextItem[aElemCount++],iOpaque.length,iOpaque.data);
			if(status != NW_STAT_SUCCESS)
			{
				rcode = KImpsErrorEncode;
			}
		}
		else
*/
		{
			iStoreValue->Des().Append(0);
			NW_String_initialize(&iString,(TUint8*)iStoreValue->Des().Ptr(),HTTP_utf_8);
			status = NW_DOM_TextItem_initFromString(&iTextItem[aElemCount++],&iString);
			if(status != NW_STAT_SUCCESS)
			{
				rcode = KImpsErrorEncode;
			}
		}

    }
    else
    {
        rcode = KErrNotFound;
    }

    return(rcode);
    
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeExt
// method stores extension value to DOM node
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeExtL(TInt aToken,TInt& aElemCount)
    {
    
    NW_Status_t status = NW_STAT_SUCCESS;
    TInt rcode = KErrNone;
    TInt elem = 0;
    TInt value = 0;
    TInt maxlength;

    TDesC8 *p8;
    if(iImpsData->RestoreDesc8L(iAccessKey,p8))
    {
        if((elem = iWbXmlData->GetWbXmlElementValue(aToken,value,maxlength))
                 != KImpsWbXmlDataNotFound)
        {
            StoreInteger(elem,iStoreValue->Des());
            iString.storage = (TUint8*)iStoreValue->Ptr();
            iString.length = iStoreValue->Length();
        }
        if(maxlength && p8->Length() > maxlength)
        {
            rcode = KImpsErrorEncode;
        }
        else
        {
            if(p8->Length() >= iStoreValue->Des().MaxLength())
            {
                delete iStoreValue;
                iStoreValue = NULL;
                iStoreValue = HBufC8::NewL(p8->Length() + 1);
            }
            iStoreValue->Des().Copy(*p8);
            iStoreValue->Des().Append(0);
            NW_String_initialize(&iString,(TUint8*)iStoreValue->Des().Ptr(),HTTP_utf_8);
            status = NW_DOM_TextItem_initFromString(&iTextItem[aElemCount++],&iString);
            if(status != NW_STAT_SUCCESS)
            {
                rcode = KImpsErrorEncode;
            }
        }
    }
    else
    {
        rcode = KErrNotFound;
    }

    return(rcode);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeIntegerL
// method stores integer value to DOM node
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeIntegerL(TInt aToken,TInt& aElemCount)
    {
    
    NW_Status_t status = NW_STAT_SUCCESS;
    TInt rcode = KErrNone;
    TInt elem;
    TInt value;
    TInt maxlength;

    if(iImpsData->RestoreIntegerL(iAccessKey,value))
    {
        elem = iWbXmlData->GetWbXmlElementValue(aToken,value,maxlength);
        if(elem == KImpsWbXmlDataNotFound)
        {
            TPtrC8 p = iWbXmlData->GetWbXmlStringValue(aToken,value);
            if(p.Length())
            {
                NW_String_initialize(&iString,(TUint8*)p.Ptr(),HTTP_utf_8);
                status = NW_DOM_TextItem_initFromString(&iTextItem[aElemCount++],&iString);
                if(status != NW_STAT_SUCCESS)
                {
                    rcode = KImpsErrorEncode;
                }
            }
            else
            {
                iStoreValue->Des().Zero();
                StoreInteger(value,iStoreValue->Des());
                iOpaque.data = (TUint8*)iStoreValue->Ptr();
                iOpaque.length = iStoreValue->Length();
                status = NW_DOM_TextItem_initFromOpaque(&iTextItem[aElemCount++],iOpaque.length,iOpaque.data);
                if(status != NW_STAT_SUCCESS)
                {
                    rcode = KImpsErrorEncode;
                }
            }
        }
        else
        {
            status = NW_DOM_TextItem_initFromExtensionInt(&iTextItem[aElemCount++],NW_WBXML_EXT_T_0,elem);
            if(status != NW_STAT_SUCCESS)
            {
                rcode = KImpsErrorEncode;
            }
        }
    }
    else
    {
        rcode = KErrNotFound;
    }

    return(rcode);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::SearchPrefix
// method searches prefixes from given string
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::SearchPrefix(TPtrC8 aString,TInt& aOffset)
    {
    
    TPtrC8 ptr(KImpsHttps8);
    if(aString.Find(ptr) ==  0)
    {
        aOffset = ptr.Length();
        return(KImpsWbXmlHttps);
    }

    ptr.Set(KImpsHttp8);
    if(aString.Find(ptr) == 0)
    {
        aOffset = ptr.Length();
        return(KImpsWbXmlHttp);
    }

    return(KErrNotFound);
    
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::EncodeBooleanL
// method stores boolean value to DOM node
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::EncodeBooleanL(TInt aToken,TInt& aElemCount)
    {
    
    NW_Status_t status = NW_STAT_SUCCESS;
    TInt elem;
    TInt value;
    TInt maxlength;
    TInt rcode = KErrNone;

    if(iImpsData->RestoreBooleanL(iAccessKey,value))
    {
        elem = iWbXmlData->GetWbXmlElementValue(aToken,value,maxlength);
        if(elem == KImpsWbXmlDataNotFound)
        {
            rcode = KErrNotFound;
        }
        else
        {
            status = NW_DOM_TextItem_initFromExtensionInt(&iTextItem[aElemCount++],NW_WBXML_EXT_T_0,elem);
            if(status != NW_STAT_SUCCESS)
            {
                rcode = KImpsErrorEncode;
            }
        }
    }
    else
    {
        rcode = KErrNotFound;
    }

    return(rcode);
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::ReadNextValue
// method reads next value in element declaration
// ---------------------------------------------------------
//
TPtrC CImpsEncodeWbXml::ReadNextValue(SImpsValueList* aValueList,TPtrC aValues)
    {

    aValueList->Flags &= KImpsValueListAlternative;
    TBool end = EFalse;

    while(aValues[aValueList->Next] == KImpsSpace) aValueList->Next++;
    aValueList->Current = aValueList->Next;

    TInt length = 0;
    while(aValues[aValueList->Next] != ',' && 
          aValues[aValueList->Next] != ')' &&
          aValues[aValueList->Next] != '|')
    {
    
        if(aValues[aValueList->Next] == '?')
        {
            aValueList->Flags |= KImpsValueListOptional;
            end = ETrue;
        }

        if(aValues[aValueList->Next] == '+')
        {
            aValueList->Flags |= KImpsValueListOnceOrMore;
            end = ETrue;
        }
        if(aValues[aValueList->Next] == '*')
        {
            aValueList->Flags |= KImpsValueListMultiple;
            end = ETrue;
        }
        if(aValues[aValueList->Next] == KImpsSpace)
        {
            end = ETrue;
        }
        aValueList->Next++;
        if(end == EFalse) length++;
    }
    
    if(aValues[aValueList->Next] == '|')
    {
        aValueList->Flags |= KImpsValueListAlternative;
    }

    if(aValues[aValueList->Next] == ')')
    {
        aValueList->Flags |= KImpsValueListEnd;
    }
    aValueList->Next++;

    return(aValues.Mid(aValueList->Current,length));
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::MimeType
// method returns wbxml message MIME type
// ---------------------------------------------------------
//
TPtrC8 CImpsEncodeWbXml::MimeType( )
    {
    
    return TPtrC8(KImpsWbXmlMessageMimeType);
    
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::CountContentSize
// method count string size
// ---------------------------------------------------------
//
TInt CImpsEncodeWbXml::CountContentSize(TDesC*& aPtr)
    {

    TInt size = 0;
    for (TInt j = 0; j < aPtr->Length(); j++)
    {          
        TChar byte = TChar(aPtr->Ptr()[j]);
        if ( byte < 0x0080)
            size += 0;
        else if ( byte < 0x0800)
            size += 1;
        else if ( byte < 0x10000)
            size += 2;
        else if ( byte < 0x20000)
            size += 3;
        else if ( byte < 0x4000000)
            size += 4;
        else
            size += 5;
     }
     size += aPtr->Length();
     return(size);
    }

//
// ---------------------------------------------------------
// NewEncoderL()
// wbxml encoder create function
// Returns: C-class entity of abstract MImpsEncoder class
// ---------------------------------------------------------
//

EXPORT_C GLDEF_C MImpsEncoder* NewEncoderL( )
    {
    
    return (MImpsEncoder*) CImpsEncodeWbXml::NewL( );
    
    }

// ---------------------------------------------------------
// CImpsEncodeWbXml::Destroy
// message encoder destructor
// ---------------------------------------------------------
//
void CImpsEncodeWbXml::Destroy( )
    {
    
    delete this;
    
    }

//  End of File  

