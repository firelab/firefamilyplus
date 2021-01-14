#pragma once
class CFireplusSet;
class CStatCauseSet;
class CDatabase;

class utils
{
public:
	utils(CDatabase *_pDB = NULL);
	~utils(void);
	bool validateStatCause(int val);
	int TranslateCauseToUSFS(int cause, int agency);
	int TranslateCauseToDOI(int cause, int agency);
	CStatCauseSet *statCauseSet;
	CDatabase *pDB;
};

