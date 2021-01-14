#include "stdafx.h"
#include "utils.h"
#include "stdafx.h"
#include "fireplusSet.h"
#include "StatCauseSet.h"
 

utils::utils(CDatabase *_pDB)
{
	statCauseSet = 0;
	pDB = _pDB;
}


utils::~utils(void)
{
	 if (statCauseSet)
	 {
		 statCauseSet->Close();
		 delete statCauseSet;
	 }
}


// new 2014
bool utils::validateStatCause(int val)
{

	bool validate = false;
	if (! statCauseSet)
	{
		statCauseSet = new CStatCauseSet(pDB);
		
		

	}
	if (! statCauseSet->IsOpen())
		statCauseSet->Open();
	statCauseSet->MoveFirst();

	while (! statCauseSet->IsEOF())
		{
			if (val == statCauseSet->m_StatisticalCause)
				validate = true;

			statCauseSet->MoveNext();

		}
	statCauseSet->Close();


	return validate;
}

/* new 9/2006: translate cause code to USFS.
   (if agency is USFS, return existing code
    if agency is DOI (agencyID > 1), translate to USFS equivalent 
	*/

int utils::TranslateCauseToUSFS(int cause, int agency){

	if (agency < 2 || agency > 5)
		return cause;

	switch (cause){
		case 2:    return 4; break;   // campfire
		case 4:    return 5; break;   // debris
		case 5:    return 7; break;   // arson
		case 6:    return 2; break;   // equipment 
		case 7:    return 6; break;   // railroad

	}
 
	return cause;


}



int utils::TranslateCauseToDOI(int cause, int agency){

	if (agency >= 2 && agency <= 5)
		return cause;

	switch (cause){
		case 4:    return 2; break;   
		case 5:    return 4; break;   
		case 7:    return 5; break;   
		case 2:    return 6; break;    
		case 6:    return 7; break;   

	}
 
	return cause;


}

