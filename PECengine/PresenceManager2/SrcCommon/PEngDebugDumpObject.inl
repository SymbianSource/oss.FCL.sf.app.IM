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
* Description:  Object dumpper.
*
*/


// -----------------------------------------------------------------------------
// PEngDebugDumpObject::GatherModelDetails()
// -----------------------------------------------------------------------------
//
void PEngDebugDumpObject::GatherModelDetails( TDes& aPrintBuffer,
                                              const MPEngPresenceAttrModel2* aModel )
    {
    if( !aModel)
        {
        aPrintBuffer.Append( _L("<NULL> object") );
        }

    else
        {
        aPrintBuffer.Append( aModel->PresenceID() );



        //add attribute type ID..

        //and XML name...and namespace
        TBuf<20> xmlBuffer;
        TPtrC8 xmlPtr( NULL, 0 );
        aModel->TypeProperties().DesC8Peroperty( xmlPtr )
        xmlBuffer.Copy( xmlPtr.Left( xmlBuffer.MaxLength() );
        aPrintBuffer.Append( xmlBuffer );

        aPrintBuffer.Append( _L(".. ..") );

        aModel->TypeProperties().DesC8Peroperty( xmlPtr )
        xmlBuffer.Copy( xmlPtr.Right( xmlBuffer.MaxLength() );
        aPrintBuffer.Append( xmlBuffer );


        switch( aModel->DataSource() )
            {
            case EPEngAttrNetwork:
                {
                aPrintBuffer.Append( _L(" [Network]") );
                break;
                }

            case EPEngAttrLocalEdited:
                {
                aPrintBuffer.Append( _L(" [LocalEdited]") );
                break;
                }

            case EPEngAttrLocalDefault:
            default:
                {
                aPrintBuffer.Append( _L(" [LocalDefault]") );
                break;
                }
            };
        }
    }


// -----------------------------------------------------------------------------
// PEngDebugDumpObject::Dump()
// -----------------------------------------------------------------------------
//
void PEngDebugDumpObject::Dump( MPEngPresenceAttrModel2* aModel )
    {
    PENG_DP( D_PENG_LIT("Attribute model dump: [1] model") );
    TBuf<200> printBuffer;
    printBuffer.AppendFormat( _L("\t %i. "), ii );
    MPEngPresenceAttrModel2* model = aModels[ ii ];
    if( !model )
        {
        printBuffer.Append( _L("<NULL> object") );
        }


// -----------------------------------------------------------------------------
// PEngDebugDumpObject::Dump()
// -----------------------------------------------------------------------------
//
void PEngDebugDumpObject::Dump( RPointerArray<MPEngPresenceAttrModel2>& aModels )
    {
    const TInt modelCount = aModels.Count();
    TPtrC lastUserId( NULL, 0 );
    TBuf<200> printBuffer;

    PENG_DP( D_PENG_LIT("Attribute model array dump: [%i] models"), modelCount  );
    for( TInt ii(0); ii <modelCount;  ii++ )
        {
        printBuffer.Zero();
        const MPEngPresenceAttrModel2* model = aModels[ ii ];

        printBuffer.AppendFormat( _L("\t %i. "), ii );

            PENG_DP( D_PENG_LIT("%S"), &printBuffer );
            }
        }
    }


// -----------------------------------------------------------------------------
// PEngDebugDumpObject::Dump()
// -----------------------------------------------------------------------------
//
void PEngDebugDumpObject::Dump( MPEngTransactionStatus2& aStatus )
    {
    PENG_DP( D_PENG_LIT("DUMPING Transaction status: status code [%i]"), aStatus.Status() );

    const TInt detailedCount = aStatus.DetaildResultCount();
    for( TInt ii(0); ii <detailedCount; ii++ )
        {
        TBuf<200> printBuffer;
        printBuffer.AppendFormat( _L("\t %i. "), ii );

        const MPEngDetailedResultEntry* dEntry = aStatus.DetailedResult( ii );

        TPtrC desc( NULL, 0 );
        if( dEntry->GetDetailedDesc( desc, EPEngDTPresenceID ) == KErrNone )
            {
            desc.Set(desc.Left(50));
            printBuffer.AppendFormat( _L("[%S]"), &desc );
            }
        else
            {
            printBuffer.Append( _L("[none]") );
            }

        if( dEntry->GetDetailedDesc( desc, EPEngDTContactListID ) == KErrNone )
            {
            desc.Set(desc.Left(50));
            printBuffer.AppendFormat( _L("[%s]"), &desc );
            }
        else
            {
            printBuffer.Append( _L("[none]") );
            }

        printBuffer.AppendFormat( _L("[%i]"), dEntry->Error() );

        if( dEntry->GetDetailedDesc( desc, EPEngDTErrorDescription ) == KErrNone )
            {
            desc.Set(desc.Left(50));
            printBuffer.AppendFormat( _L("[%s]"), &desc );
            }
        else
            {
            printBuffer.Append( _L("[none]") );
            }

        PENG_DP( D_PENG_LIT("%S"), &printBuffer );
        }
    }


//  End of File

