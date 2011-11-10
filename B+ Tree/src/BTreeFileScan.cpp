#include "minirel.h"
#include "bufmgr.h"
#include "db.h"
#include "new_error.h"
#include "BTreeFile.h"
#include "BTreeFileScan.h"

//-------------------------------------------------------------------
// BTreeFileScan::~BTreeFileScan
//
// Input   : None
// Output  : None
// Purpose : Clean Up the B+ tree scan.
//-------------------------------------------------------------------
BTreeFileScan::~BTreeFileScan ()
{
	//TODO: add your code here
	//...
}


//-------------------------------------------------------------------
// BTreeFileScan::BTreeFileScan
//
// Input   : None
// Output  : None
// Purpose : Constructs a new BTreeFileScan object. Note that this constructor
//           is private and can only be called from 
//-------------------------------------------------------------------
BTreeFileScan::BTreeFileScan() {
	//Add our code here. 
	//...
}

//-------------------------------------------------------------------
// BTreeFileScan::GetNext
//
// Input   : None
// Output  : rid  - record id of the scanned record.
//           keyPtr - and a pointer to it's key value.
// Purpose : Return the next record from the B+-tree index.
// Return  : OK if successful, DONE if no more records to read
//           or if high key has been passed.
//-------------------------------------------------------------------

Status BTreeFileScan::GetNext (RecordID & rid, char*& keyPtr)
{ 
	//Add your code here.
    if(this->done){
		return DONE;
    }
    //  LeafPage* currentPage = NULL;
    PIN((currentRecord.pageNo),currentPage);
    while(!(this->done)){
		//char * k;
		Status s = iter->GetNext(keyPtr, rid); //NEXT???   NEED to check if Fail
        if(s != DONE){
			if(this->highKey == NULL || strcmp(keyPtr,this->highKey) <= 0){
			//withing upper bound
				if(this->lowKey == NULL || strcmp(keyPtr,this->lowKey) >= 0){
					//within lower bount
					MINIBASE_BM->UnpinPage(currentPage->PageNo());
	              	return OK;
	            }else{
					continue; //haven't reached range yet
	            }
	        }else{
				//exceeded upper bound
				this->done = true;
	            rid.pageNo = INVALID_PAGE;
	           	rid.slotNo = -1;
	            UNPIN((currentPage->PageNo()), CLEAN);
	            return DONE;
	        }
		}
        // s == DONE
        else{
        	// go to next page
			currentRecord.pageNo = currentPage->GetNextPage();
        	currentRecord.slotNo = -1;
            if(currentRecord.pageNo == INVALID_PAGE){
			//no more pages
				this->done = true;
                UNPIN(currentPage->PageNo(), CLEAN);
                return DONE;
            }
            UNPIN(currentPage->PageNo(), CLEAN);
            if(MINIBASE_BM->PinPage(currentRecord.pageNo, (Page*&)currentPage)!=OK){
				// ???
				this->done = true;
	            rid.pageNo = INVALID_PAGE;
        	    rid.slotNo = -1;
            	return DONE;
            }else{
			// Do check process above again???
			//change iter
			}
		}
    }
	return FAIL;//!!!!!! shouldn't be needed???
}


//-------------------------------------------------------------------
// BTreeFileScan::DeleteCurrent
//
// Input   : None
// Output  : None
// Purpose : Delete the entry currently being scanned (i.e. returned
//           by previous call of GetNext()). Note that this method should
//           call delete on the page containing the previous key, but it 
//           does (and should) NOT need to redistribute or merge keys. 
// Return  : OK 
//-------------------------------------------------------------------
Status BTreeFileScan::DeleteCurrent () {  
	//Add your code here. 
    if(done){
		return DONE;
    }
    //LeafPage* currentPage = NULL;
    PIN(currentRecord.pageNo, currentPage);
    iter->DeleteCurrent();
    MINIBASE_BM->UnpinPage(currentRecord.pageNo);
    return OK;

	//return FAIL;
}

Status BTreeFileScan::_SetIter() {  
    PIN((currentRecord.pageNo),currentPage);
	PageKVScan<RecordID>* scan = new PageKVScan<RecordID>();
	if((currentPage->Search(lowKey, *scan))==DONE){ //check if fail?
		//move one more?
	}
	UNPIN((currentRecord.pageNo), CLEAN);
	return OK;
}