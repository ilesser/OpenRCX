///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ADS_EXTRCAP_H
#define ADS_EXTRCAP_H

#ifndef ADS_H
//#include "ads.h"
#include <ads.h>
#endif

#include "db.h"
//#include "dbUtil.h"
//#include "dbShape.h"
//#include "dbExtControl.h"
//#include "util.h"
//#include "darr.h"
#include <dbUtil.h>
#include <dbShape.h>
#include <dbExtControl.h>
#include <util.h>
#include <darr.h>

//#include "gz.h"
#include <gz.h>

#ifndef ADS_ZOBJECT_H
//#include "ZObject.h"
#include <ZObject.h>
#endif

//#include "ISdb.h"
#include "IZdcr.h" 
//#include "IZgui.h" 
#include "ZguiExt.h" 
#include <ISdb.h>
//#include <IZdcr.">
#include <IZgui.h>

#include "extprocess.h" 
//#include "gseq.h" 
#include <gseq.h> 

#ifndef _WIN32
#define D2I_ROUND (name) rint(name)
#else
#define D2I_ROUND (name) name
#endif

//#define NEW_GS_FLOW

#define HI_ACC_1
#ifdef HI_ACC_1
#define HI_ACC_2
#define DAVID_ACC_08_02
#define GS_CROSS_LINES_ONLY 1
#define BUG_LAYER_CNT 1
#endif

#ifdef _WIN32
#define AFILE FILE
#define ATH__fprintf fprintf
#define ATH__fopen fopen
#define ATH__fclose fclose
#endif

#include <map>
BEGIN_NAMESPACE_ADS
class extMeasure;

int AthResourceLog(char *title, int smallScale);
int Ath__double2int(double);

class extMetBox  // assume cross-section on the z-direction
{
	int _bot[3];
	int _top[3];
	uint _botDX;
	uint _topDX;
};

class ext2dBox  // assume cross-section on the z-direction
{
	int _ll[2];
	int _ur[2];
	uint _met;
	uint _dir;
	uint _id;
	uint _map;
	
	void rotate();
	bool matchCoords(int *ll, int *ur);
	void printGeoms3D(FILE *fp, double h, double t, int *orig);
	uint length();
	uint width();
	int loX();
	int loY();
	uint id();

	friend class extMeasure;
	friend class extRCModel;

};
class extGeoVarTable
{
	int _x;
	int _y;
	double _nominal;
	double _epsilon;
	char *_layerName;
	Ath__array1D<double> *_diffTable;
	Ath__array1D<double> *_varCoeffTable;
	bool _fractionDiff;
	bool _simpleVersion;

public:
	extGeoVarTable(int x, int y, double nom, double e, Ath__array1D<double> *A, bool simpleVersion, bool calcDiff);
	~extGeoVarTable();
	double getVal(uint n, double &nom);
	int getLowerBound(uint dir);
	bool getThicknessDiff(int n, double &delta_th);
};

class extGeoThickTable
{
	char *_layerName;
	Ath__array1D<uint> *_widthTable;
	uint _rowCnt;
	uint _colCnt;
	uint _tileSize;
	int _ll[2];
	int _ur[2];

	extGeoVarTable ***_thickTable;
public:
	extGeoThickTable(int x1, int y1, int x2, int y2, uint tileSize, Ath__array1D<double> *A, uint units);
	~extGeoThickTable();
	extGeoVarTable* addVarTable(int x, int y, double nom, double e, Ath__array1D<double> *A, bool simpleVersion, bool calcDiff);
	bool getThicknessDiff(int x, int y, uint w, double &delta_th);
	extGeoVarTable* getSquare(int x, int y, uint *rowCol);
	int getLowerBound(uint dir, uint *rowCol);
	int getUpperBound(uint dir, uint *rowCol);
	int getRowCol(int xy, int base, uint bucket, uint bound);
};


class extSpef;

class extDistRC
{
private:

	//double _dist;
	int _sep;
	double _coupling;
	double _fringe;
	double _diag;
	double _res;
	
public:
	void set(uint d, double cc, double fr, double a, double r);
	void readRC(Ath__parser *parser, double dbFactor=1.0);
	double getFringe();
	double getCoupling();
	double getDiag();
	double getRes();
	void addRC(extDistRC *rcUnit, uint len, bool addCC);
	void writeRC(AFILE *fp, bool bin);
	void interpolate(uint d, extDistRC* rc1, extDistRC* rc2);

	friend class extDistRCTable;
	friend class extDistWidthRCTable;
	friend class extMeasure;
	friend class extMain;
};
class extDistRCTable
{
private:

	//Ath__array1D<double> *_distTable;
	//Ath__array1D<uint> *_distMapTable;

	Ath__array1D<extDistRC*> *_measureTable;
	Ath__array1D<extDistRC*> *_computeTable;
	uint _unit;

	void makeCapTableOver();
	void makeCapTableUnder();

public:
	extDistRCTable(uint distCnt);
	~extDistRCTable();
	
	extDistRC* getRC_99();

	uint addMeasureRC(extDistRC *rc);
	void makeComputeTable(uint maxDist, uint distUnit);
	extDistRC* getLastRC();
	extDistRC* getComputeRC(double dist);
	extDistRC* getComputeRC(uint dist);
	extDistRC* getRC(uint s, bool compute);
	uint writeRules(AFILE *fp, Ath__array1D<extDistRC*> *table, double w, bool bin);
	uint writeRules(AFILE *fp, double w, bool compute, bool bin);
	uint writeDiagRules(AFILE *fp, Ath__array1D<extDistRC*> *table, double w1, double w2, double s, bool bin);
	uint writeDiagRules(AFILE *fp, double w1, double w2, double s, bool compute, bool bin);
	uint readRules(Ath__parser *parser, AthPool<extDistRC> *rcPool, bool compute, bool bin, bool ignore, double dbFactor=1.0);
	uint interpolate(uint distUnit, int maxDist, AthPool<extDistRC> *rcPool);
	uint mapInterpolate(extDistRC *rc1, extDistRC *rc2, uint distUnit, int maxDist, AthPool<extDistRC> *rcPool);
	uint mapExtrapolate(uint loDist, extDistRC *rc2, uint distUnit, AthPool<extDistRC> *rcPool);

	void getFringeTable(Ath__array1D<int> *sTable, Ath__array1D<double> *rcTable, bool compute);

};
class extDistWidthRCTable
{
public:

	bool _ouReadReverse;
	bool _over;
	uint _layerCnt;
	uint _met;

	Ath__array1D<int> *_widthTable;
	Ath__array1D<uint> *_widthMapTable;
	Ath__array1D<int> *_diagWidthTable[12];
	Ath__array1D<int> *_diagDistTable[12];
	Ath__array1D<uint>*_diagWidthMapTable[12];
	Ath__array1D<uint> *_diagDistMapTable[12];

	uint _modulo;
	int _firstWidth;
	int _lastWidth;
	Ath__array1D<int> *_firstDiagWidth;
	Ath__array1D<int> *_lastDiagWidth;
	Ath__array1D<int> *_firstDiagDist;
	Ath__array1D<int> *_lastDiagDist;
	bool _widthTableAllocFlag; // if false widthtable is pointer

	extDistRCTable ***_rcDistTable; // per over/under metal, per width
	extDistRCTable *****_rcDiagDistTable;
	uint _metCnt; // if _over==false _metCnt???

	AthPool<extDistRC> *_rcPoolPtr;
	extDistRC *_rc31;

public:
	extDistWidthRCTable(bool dummy, uint met, uint layerCnt, uint width);
	extDistWidthRCTable(bool over, uint met, uint layerCnt, uint metCnt, Ath__array1D<double> *widthTable, AthPool<extDistRC> *rcPool, double dbFactor=1.0);
	extDistWidthRCTable(bool over, uint met, uint layerCnt, uint metCnt, Ath__array1D<double> *widthTable, int diagWidthCnt, int diagDistCnt, AthPool<extDistRC> *rcPool, double dbFactor=1.0);
	extDistWidthRCTable(bool over, uint met, uint layerCnt, uint metCnt, uint maxWidthCnt, AthPool<extDistRC> *rcPool);
	void addRCw(uint n, uint w, extDistRC *rc);
	void createWidthMap();
	void makeWSmapping();

	~extDistWidthRCTable();
	void setDiagUnderTables(uint met, Ath__array1D<double> *diagWidthTable, Ath__array1D<double> *diagDistTable, double dbFactor=1.0);
	uint getWidthIndex(uint w);
	uint getDiagWidthIndex(uint m, uint w);
	uint getDiagDistIndex(uint m, uint s);
	uint addCapOver(uint met, uint metUnder, extDistRC *rc);
	extDistRC* getCapOver(uint met, uint metUnder);
	uint writeWidthTable(AFILE *fp, bool bin);
	uint writeDiagWidthTable(AFILE *fp, uint met, bool bin);
	uint writeDiagDistTable(AFILE *fp, uint met, bool bin);
	void writeDiagTablesCnt(AFILE *fp, uint met, bool bin);
	uint writeRulesOver(AFILE *fp, bool bin);
	uint writeRulesUnder(AFILE *fp, bool bin);
	uint writeRulesDiagUnder(AFILE *fp, bool bin);
	uint writeRulesDiagUnder2(AFILE *fp, bool bin);
	uint writeRulesOverUnder(AFILE *fp, bool bin);
	uint getMetIndexUnder(uint mOver);
	uint readRulesOver(Ath__parser *parser, uint widthCnt, bool bin, bool ignore, double dbFactor=1.0);
	uint readRulesUnder(Ath__parser *parser, uint widthCnt, bool bin, bool ignore, double dbFactor=1.0);
	uint readRulesDiagUnder(Ath__parser *parser, uint widthCnt, uint diagWidthCnt, uint diagDistCnt, bool bin, bool ignore, double dbFactor=1.0);
	uint readRulesDiagUnder(Ath__parser *parser, uint widthCnt, bool bin, bool ignore, double dbFactor=1.0);
	uint readRulesOverUnder(Ath__parser *parser, uint widthCnt, bool bin, bool ignore, double dbFactor=1.0);
	uint readMetalHeader(Ath__parser *parser, uint &met, char *keyword, bool bin, bool ignore);

	//extDistRC* getRC(uint mou, double w, double s);
	extDistRC* getRC(uint mou, uint w, uint s);
	extDistRC* getRC(uint mou, uint w,uint dw, uint ds, uint s);
	extDistRC* getFringeRC(uint mou, uint w);
	void getFringeTable(uint mou, uint w, Ath__array1D<int> *sTable, Ath__array1D<double> *rcTable, bool map);

	extDistRC* getLastWidthFringeRC(uint mou);
	extDistRC* getRC_99(uint mou, uint w, uint dw, uint ds);
};
class extMetRCTable
{
public:
	
	uint _layerCnt;
	char _name[128];
	extDistWidthRCTable **_capOver;
	extDistWidthRCTable **_capUnder;
	extDistWidthRCTable **_capOverUnder;
	extDistWidthRCTable **_capDiagUnder;

	AthPool<extDistRC>* _rcPoolPtr;
	double _rate;
public:
	extMetRCTable(uint layerCnt, AthPool<extDistRC> *rcPool);
	~extMetRCTable();
	void allocateInitialTables(uint layerCnt, uint widthCnt, bool over, bool under, bool diag=false);
	void allocOverTable(uint met, Ath__array1D<double> *wTable, double dbFactor=1.0);
	void allocDiagUnderTable(uint met, Ath__array1D<double> *wTable, int diagWidthCnt, int diagDistCnt, double dbFactor=1.0);
	void allocDiagUnderTable(uint met, Ath__array1D<double> *wTable, double dbFactor=1.0);
	void allocUnderTable(uint met, Ath__array1D<double> *wTable, double dbFactor=1.0);
	void allocOverUnderTable(uint met, Ath__array1D<double> *wTable, double dbFactor=1.0);
	void setDiagUnderTables(uint met, uint overMet, Ath__array1D<double> *diagWTable, Ath__array1D<double> *diagSTable, double dbFactor=1.0);
	void addRCw(extMeasure *m);
	uint readRCstats(Ath__parser *parser);
	void mkWidthAndSpaceMappings();

	uint addCapOver(uint met, uint metUnder, extDistRC *rc);
	uint addCapUnder(uint met, uint metOver, extDistRC *rc);
	extDistRC* getCapOver(uint met, uint metUnder);
	extDistRC* getCapUnder(uint met, uint metOver);
	extDistRC* getOverFringeRC(extMeasure *m);
};
class extRCTable
{
private:

	bool _over;
	uint _maxCnt1;
	Ath__array1D<extDistRC*> ***_inTable; // per metal per width
	Ath__array1D<extDistRC*> ***_table;

	void makeCapTableOver();
	void makeCapTableUnder();

public:
	extRCTable(bool over, uint layerCnt);
	uint addCapOver(uint met, uint metUnder, extDistRC *rc);
	extDistRC* getCapOver(uint met, uint metUnder);
};

class extMeasure;
class extMainOptions;

class extRCModel
{
private:

	bool _ouReadReverse;
	uint _layerCnt;
	char _name[128];
	
	int _noVariationIndex;
	uint _modelCnt;
	Ath__array1D<double> *_dataRateTable;
	Ath__array1D<int> *_dataRateTableMap;
	extMetRCTable **_modelTable;
	uint _tmpDataRate;
	bool _diag;
	bool _verticalDiag;
	bool _maxMinFlag;
	uint _diagModel;
	bool _keepFile;
	uint _metLevel;

	extRCTable *_capOver;
	extRCTable *_capDiagUnder;
	extRCTable *_capUnder;
	AthPool<extDistRC> *_rcPoolPtr;
	extProcess *_process;
	char* _ruleFileName;
	char* _wireFileName;
	char* _wireDirName;
	char* _topDir;
	char* _patternName;
	Ath__parser *_parser;
	char* _solverFileName;

	FILE* _capLogFP;
	FILE* _logFP;

	bool _writeFiles;
	bool _readSolver;
	bool _runSolver;

	bool _readCapLog;
	char _commentLine[10000];
	bool _commentFlag;

	uint *_singlePlaneLayerMap;
	uint **_overUnderPlaneLayerMap;

	extMain *_extMain;

public:

	extMetRCTable * getMetRCTable(uint ii) { return _modelTable[ii]; };

	int getModelCnt() { return _modelCnt; };
	int getLayerCnt() { return _layerCnt; };
	int getDiagModel() {return _diagModel;};
	bool getVerticalDiagFlag () {return _verticalDiag;};
	void setDiagModel(uint i) { _diagModel=i;}
	extRCModel(uint layerCnt, char *name);
	extRCModel(char *name);
	extProcess* getProcess();
	uint findBiggestDatarateIndex(double d);
	int findVariationZero(double d);
	int findClosestDataRate(uint n, double diff);
	~extRCModel();
	void setExtMain(extMain *x);
	void createModelTable(uint n, uint layerCnt);

	uint addLefTotRC(uint met, uint underMet, double fr, double r);
	uint addCapOver(uint met, uint underMet, uint d, double cc, double fr, double a, double r);
	double getTotCapOverSub(uint met);
	double getRes(uint met);

	uint benchWithVar_density(extMainOptions *opt, extMeasure *measure);
	uint benchWithVar_lists(extMainOptions *opt, extMeasure *measure);

	uint readCapacitanceBench3D(bool readCapLog, extMeasure *m, bool skipPrintWires);
	bool measureNetPattern(extMeasure *m, uint shapeId, Ath__array1D<ext2dBox*> *boxArray);
	uint writePatternGeoms(extMeasure *m, Ath__array1D<ext2dBox*> *boxArray);
	bool makePatternNet3D(extMeasure *measure, Ath__array1D<ext2dBox*> *boxArray);
	uint netWiresBench(extMainOptions *opt, extMain *xMain, uint netId, ZPtr<ISdb> netSearch);
	uint runWiresSolver(uint netId, int shapeId);
	uint getNetCapMatrixValues3D(uint nodeCnt, uint shapeId, extMeasure *m);



	void setProcess(extProcess *p);
	void setDataRateTable(uint met);
	uint singleLineOver(uint widthCnt);
	uint twoLineOver(uint widthCnt, uint spaceCnt);
	uint threeLineOver(uint widthCnt, uint spaceCnt);
	uint getCapValues(uint lastNode, double &cc1, double &cc2, double &fr, double &tot, extMeasure *m);
	uint getCapValues3D(uint lastNode, double &cc1, double &cc2, double &fr, double &tot, extMeasure *m);
	uint getCapMatrixValues(uint lastNode, extMeasure *m);
	uint getCapMatrixValues3D(uint lastNode, extMeasure *m);
	uint readCapacitance(uint wireNum, double &cc1, double &cc2, double &fr, double &tot, bool readCapLog, extMeasure *m=NULL);
	uint readCapacitanceBench(bool readCapLog, extMeasure *m);
	uint readCapacitanceBenchDiag(bool readCapLog, extMeasure *m);
	uint readCapacitanceBench3D(bool readCapLog, extMeasure *m);
	extDistRC *readCap(uint wireCnt, double w, double s, double r);
	uint readCap(uint wireCnt, double cc1, double cc2, double fr, double tot);
	FILE* openFile(char *topDir, char *name, char *suffix, char *permissions);
	FILE* openSolverFile();
	void mkFileNames(extMeasure *m, char *wiresNameSuffix);
	void writeWires(FILE *fp, extMasterConductor *m, uint wireCnt, double X, double w, double s);
	void writeWires(FILE *fp, extMeasure *measure, uint wireCnt);
	void writeWires2(FILE *fp, extMeasure *measure, uint wireCnt);
	void writeRuleWires(FILE *fp, extMeasure *measure, uint wireCnt);
	void writeWires2_3D(FILE *fp, extMeasure *measure, uint wireCnt);
	void writeRuleWires_3D(FILE *fp, extMeasure *measure, uint wireCnt);
	int writeBenchWires(FILE *fp, extMeasure *measure);
	void writeRaphaelCaps(FILE *fp, extMeasure *measure, uint wireCnt);
	void writeRaphaelCaps3D(FILE *fp, extMeasure *measure, uint wireCnt);
	void setOptions(const char *topDir, const char *pattern, bool writeFiles, bool readSolver, bool runSolver);
	void setOptions(const char *topDir, const char *pattern, bool writeFiles, bool readSolver, bool runSolver, bool keepFile, uint metLevel=0);
	void runSolver(char* solverOption);
	bool solverStep(extMeasure *m);
	void cleanFiles();

	extDistRC *measurePattern(uint met, int underMet, int overMet, double width, double space, uint wireCnt, char* ou, bool readCapLog);
	bool measurePatternVar(extMeasure *m, double top_width, double bot_width, double thickness, uint wireCnt, char *wiresNameSuffix, double res=0.0);
	uint measureDiagWithVar(extMeasure *measure);
	double measureResistance(extMeasure *m, double ro, double top_widthR, double bot_widthR, double thicknessR);

	uint linesOverBench(extMainOptions *opt);
	uint linesUnderBench(extMainOptions *opt);
	uint linesDiagUnderBench(extMainOptions *opt);
	uint linesOverUnderBench(extMainOptions *opt);

	uint benchWithVar(extMeasure *measure);
	uint measureWithVar(extMeasure *m);
	void addRC(extMeasure *m);
	uint getOverUnderIndex(extMeasure *m, uint maxCnt);
	uint getUnderIndex(extMeasure *m);
	extDistWidthRCTable* getWidthDistRCtable(uint met, int mUnder, int mOver, int &n, double dRate);

	// void printCommentLine(char commentChar, double w, double width, double s, double spacing, double h, double height, double t, double thickeness);
	void printCommentLine(char commentChar, extMeasure *m);

	uint linesOver(uint wireCnt, uint widthCnt, uint spaceCnt, uint dCnt, uint met=0);
	uint linesDiagUnder(uint wireCnt, uint widthCnt, uint spaceCnt, uint dCnt, uint met=0);
	uint linesUnder(uint wireCnt, uint widthCnt, uint spaceCnt, uint dCnt, uint met=0);
	uint linesOverUnder(uint wireCnt, uint widthCnt, uint spaceCnt, uint dCnt, uint met=0);

	void allocOverTable(extMeasure *measure);
	void allocDiagUnderTable(extMeasure *measure);
	void allocUnderTable(extMeasure *measure);
	void allocOverUnderTable(extMeasure *measure);
	void computeTables(extMeasure *m, uint wireCnt, uint widthCnt, uint spaceCnt, uint dCnt);

	void getDiagTables(extMeasure *m, uint widthCnt, uint spaceCnt);
	void setDiagUnderTables(extMeasure *m);
	void getRCtable(uint met, int mUnder, int OverMet, double w, double dRate);
	void getRCtable(Ath__array1D<int> *sTable, Ath__array1D<double> *rcTable, uint valType, uint met, int mUnder, int mOver, int width, double dRate);


	void writeRules(char *name, bool binary);
	bool readRules(char *name, bool binary, bool over, bool under, bool overUnder, bool diag, uint cornerCnt=0, uint *cornerTable=NULL, double dbFactor=1.0);
	uint readMetalHeader(Ath__parser *parser, uint &met, char *keyword, bool bin, bool ignore);
	Ath__array1D<double>* readHeaderAndWidth(Ath__parser *parser, uint &met, char *ouKey, char *wKey, bool bin, bool ignore);
	uint readRules(Ath__parser *parser, uint m, uint ii, char *ouKey, char *wKey, bool over, bool under, bool bin, bool diag, bool ignore, double dbFactor=1.0);

	extDistRC* getOverFringeRC(uint met, uint underMet, uint width);
	double getFringeOver(uint met, uint mUnder, uint w, uint s);
	double getCouplingOver(uint met, uint mUnder, uint w, uint s);
	extDistRC* getOverRC(extMeasure *m);
	extDistRC* getUnderRC(extMeasure *m);
	extDistRC* getOverUnderRC(extMeasure *m);

	extDistRC* getOverFringeRC(extMeasure *m);
	extDistRC* getOverUnderFringeRC(extMeasure *m);
	extDistRC* getUnderFringeRC(extMeasure *m);

	int findDatarateIndex(double d);

	uint linesUnder(uint wireCnt, uint widthCnt, uint spaceCnt);
	FILE *mkPatternFile();
	bool openCapLogFile();
	void closeCapLogFile();
	void closeFiles();
	
	void setRuleFileName(char *name) { _ruleFileName = name;}
	char* getRuleFileName() { return _ruleFileName;}
	uint getMaxCnt(int met) { return _modelTable[_tmpDataRate]->_capOverUnder[met]->_metCnt; }

};
class extNetStats 
{
public:
	double _tcap[2];
	double _ccap[2];
	double _cc2tcap[2];
	double _cc[2];
	int _len[2];
	int _layerCnt[2];
	int _wCnt[2];
	int _vCnt[2];
	int _resCnt[2];
	double _res[2];
	int _ccCnt[2];
	int _gndCnt[2];
	int _termCnt[2];
	int _btermCnt[2];
	int _id;
	bool _layerFilter[20];
	int _ll[2];
	int _ur[2];
	void reset();
	void update_double_limits(int n, double v1, double v2, double *val, double units=1.0);
	void update_int_limits(int n, int v1, int v2, int *val, uint units=1);
	void update_double(Ath__parser *parser, const char* word, double *val, double units=1.0);
	void update_int(Ath__parser *parser, const char* word, int *val, uint units=1);
	void update_bbox(Ath__parser *parser, const char* bbox);
};
class extLenOU  // assume cross-section on the z-direction
{
public:

	int _met;
	int _underMet;
	int _overMet;
	uint _len;
	bool _over;
	bool _overUnder;
	bool _under;

	void reset();
	void addOverOrUnderLen(int met, bool over, uint len);
	void addOULen(int underMet, int overMet, uint len);

};
class extMeasure
{
public:

	extMeasure();
	~extMeasure();

	void printTraceNetInfo(const char* msg, uint netId, int rsegId);
	bool printTraceNet(const char *msg, bool init, dbCCSeg *cc=NULL, uint overSub=0, uint covered=0);

	bool parse_setLayer(Ath__parser *parser1, uint &layerNum, bool print=false);
    dbNet *createSingleWireNet(char *name, uint level, bool viaFlag, bool debug, bool skipVias, bool skipBterms=false);
	extDistRC* areaCapOverSub(uint modelNum, extMetRCTable* rcModel);

	extDistRC* getUnderLastWidthDistRC(extMetRCTable *rcModel, uint overMet);
	void createCap(int rsegId1, uint rsegId2, double *capTable);
	void areaCap(int rsegId1, uint rsegId2, uint len, uint tgtMet);
	bool verticalCap(int rsegId1, uint rsegId2, uint len, uint diagDist, uint tgtWidth, uint tgtMet);
	extDistRC* getVerticalUnderRC(extMetRCTable *rcModel, uint diagDist, uint tgtWidth, uint overMet);


	int readQcap(extMain *extmain, const char *filename, const char *design, const char *capFile, bool skipBterms, dbDatabase *db);
	int readAB(extMain *extMain, const char *filename, const char *design, const char *capFile, bool skipBterms, dbDatabase *db);
	dbRSeg *getRseg(char* netname, char* capMsg, char *tableEntryName);
	int readCapFile(const char *filename, uint &ccCnt);
	bool getFirstShape(dbNet *net, dbShape & s);

	void swap_coords(SEQ* s);
	uint swap_coords(uint initCnt, uint endCnt, Ath__array1D<SEQ*> *resTable);
	uint getOverlapSeq(uint met, SEQ* s, Ath__array1D<SEQ*> *resTable);
	uint getOverlapSeq(uint met, int *ll, int *ur, Ath__array1D<SEQ*> *resTable);

	bool isBtermConnection(dbRSeg *rseg1, dbRSeg *rseg2);
	bool isConnectedToBterm(dbRSeg *rseg1);
	uint defineBox(int *options);
	void printCoords(FILE *fp);
	void printNet(dbRSeg *rseg, uint netId);
	void updateBox(uint w_layout, uint s_layout, int dir=-1);
	void printBox(FILE *fp);
	uint initWS_box(extMainOptions *opt, uint gCnt);
	dbRSeg* getFirstDbRseg(uint netId);
	uint createNetSingleWire(char *dirName, uint idCnt, uint w_layout, uint s_layout, int dir=-1);
	uint createDiagNetSingleWire(char *dirName, uint idCnt, int base, int w_layout, int s_layout, int dir=-1);
	uint createContextNets(char *dirName, int bboxLL[2], int bboxUR[2], int met, double pitchMult);
	double getCCfringe(uint lastNode, uint n, uint start, uint end);
	double getCCfringe3D(uint lastNode, uint n, uint start, uint end);

	void updateForBench(extMainOptions *opt, extMain *extMain);
	uint measureOverUnderCap();
	uint measureOverUnderCap_orig(gs *pixelTable, uint **ouPixelTableIndexMap);
	uint getSeqOverOrUnder(Ath__array1D<SEQ*> *seqTable, gs *pixelTable, uint met, Ath__array1D<SEQ*> *resTable);
	uint computeOverOrUnderSeq(Ath__array1D<SEQ*> *seqTable, uint met, Ath__array1D<SEQ*> *resTable, bool over);
	uint computeOverUnder(int *ll, int *ur, Ath__array1D<SEQ*> *resTable);

	void release(Ath__array1D<SEQ*> *seqTable, gs *pixelTable=NULL);
	void addSeq(Ath__array1D<SEQ*> *seqTable, gs *pixelTable);
	void addSeq(int *ll, int *ur, Ath__array1D<SEQ*> *seqTable, gs *pixelTable=NULL);
	void copySeq(SEQ* t, Ath__array1D<SEQ*> *seqTable, gs *pixelTable);
	void tableCopyP(Ath__array1D<SEQ*> *src, Ath__array1D<SEQ*> *dst);
	void tableCopy(Ath__array1D<SEQ*> *src, Ath__array1D<SEQ*> *dst, gs *pixelTable);
		
	uint measureDiagFullOU();
	uint ouFlowStep(Ath__array1D<SEQ*> *overTable);
	int underFlowStep(Ath__array1D<SEQ*> *srcTable, Ath__array1D<SEQ*> *overTable);

	void measureRC(int *options);
	void computeAndStoreRC(dbRSeg *rseg1, dbRSeg *rseg2);
	void copySeqUsingPool(SEQ* t, Ath__array1D<SEQ*> *seqTable);
	void seq_release(Ath__array1D<SEQ*> *table);
	void calcOU(uint len);
	void calcRC(dbRSeg *rseg1, dbRSeg *rseg2, uint totLenCovered);
	uint computeDiag(SEQ *s, uint targetMet, uint dir, uint planeIndex, uint trackn, Ath__array1D<SEQ*> *diagTable);

	dbCCSeg *makeCcap(dbRSeg *rseg1, dbRSeg *rseg2, double ccCap);
	void addCCcap(dbCCSeg *ccap, double v, uint model);
	void addFringe(dbRSeg *rseg1, dbRSeg *rseg2, double frCap, uint model);
	void calcDiagRC(int rsegid1, uint rsegid2, uint len, uint diagWidth, uint diagDist, uint tgtMet);
	void calcDiagRC(int rsegid1, uint rsegid2, uint len, uint dist, uint tgtMet);
	int calcDist(int *ll, int *ur);

	void ccReportProgress();
	uint getOverUnderIndex();

	uint getLength(SEQ *s, int dir);
	uint blackCount(uint start, Ath__array1D<SEQ*> *resTable);
	extDistRC *computeR(uint len, double *valTable);
	extDistRC *computeOverFringe(uint overMet, uint overWidth, uint len, uint dist);
	extDistRC *computeUnderFringe(uint underMet, uint underWidth, uint len, uint dist);

	double getDiagUnderCC(extMetRCTable *rcModel, uint dist, uint overMet);
	double getDiagUnderCC(extMetRCTable *rcModel, uint diagWidth, uint diagDist, uint overMet);
	extDistRC *getDiagUnderCC2(extMetRCTable *rcModel, uint diagWidth, uint diagDist, uint overMet);
	extDistRC *computeOverUnderRC(uint len);
	extDistRC *computeOverRC(uint len);
	extDistRC *computeUnderRC(uint len);
	extDistRC *getOverUnderFringeRC(extMetRCTable *rcModel);
	extDistRC *getOverUnderRC(extMetRCTable *rcModel);
	extDistRC *getOverRC(extMetRCTable *rcModel);
	uint getUnderIndex();
	uint getUnderIndex(uint overMet);
	extDistRC *getUnderRC(extMetRCTable *rcModel);

	extDistRC *getFringe(uint len, double *valTable);

	void tableCopyP(Ath__array1D<int> *src, Ath__array1D<int> *dst);
	void getMinWidth(dbTech *tech);
	uint measureOverUnderCapCJ();
	uint computeOverUnder(int xy1, int xy2, Ath__array1D<int> *resTable);
	uint computeOUwith2planes(int *ll, int *ur, Ath__array1D<SEQ*> *resTable);
	uint intersectContextArray(int pmin, int pmax, uint met1, uint met2, Ath__array1D<int> *tgtContext);
	uint computeOverOrUnderSeq(Ath__array1D<int> *srcTable, uint met, Ath__array1D<int> *resTable, bool over);
	bool updateLengthAndExit(int &remainder, int &totCovered, int len);
	int compute_Diag_Over_Under(Ath__array1D<SEQ*> *seqTable, Ath__array1D<SEQ*> *resTable);
	int compute_Diag_OverOrUnder(Ath__array1D<SEQ*> *seqTable, bool over, uint met, Ath__array1D<SEQ*> *resTable);
	uint measureUnderOnly(bool diagFlag);
	uint measureOverOnly(bool diagFlag);
	uint measureDiagOU(uint ouLevelLimit, uint diagLevelLimit);


	uint mergeContextArray (Ath__array1D<int> *srcContext, int minS, Ath__array1D<int> *tgtContext);
	uint mergeContextArray (Ath__array1D<int> *srcContext, int minS, int pmin, int pmax, Ath__array1D<int> *tgtContext);
	uint makeMergedContextArray (uint met, int minS);
	uint makeMergedContextArray (uint met);
	uint makeMergedContextArray (int pmin, int pmax, uint met, int minS);
	uint makeMergedContextArray (int pmin, int pmax, uint met);
	uint intersectContextArray (Ath__array1D<int> *s1Context, Ath__array1D<int> *s2Context, int minS1, int minS2, Ath__array1D<int> *tgtContext);

	extDistRC *addRC(extDistRC *rcUnit, uint len, uint jj);

	void setMets(int m, int u, int o);
	void setTargetParams(double w, double s, double r, double t, double h, double w2=0.0, double s2=0.0);
	void setEffParams(double wTop, double wBot, double teff);
	void addCap();
	void printStats(FILE *fp);
	void printMets(FILE *fp);

	ext2dBox *addNew2dBox(dbNet *net, int *ll, int *ur, uint m, uint d, uint id, bool cntx);
	void clean2dBoxTable(int met, bool cntx);
	uint writeRaphael3D(FILE *fp, int met, bool cntx, double x1, double y1, double th);
	uint writeDiagRaphael3D(FILE *fp, int met, bool cntx, double x1, double y1, double th);
	void writeRaphaelPointXY(FILE *fp, double X, double Y);
	void getBox(int met, bool cntx, int &xlo, int &ylo, int &xhi, int &yhi);
	uint getBoxLength(uint ii, int met, bool cntx);

	int getDgPlaneAndTrackIndex(uint tgt_met, int trackDist, int &loTrack, int &hiTrack);
	int computeDiagOU(SEQ *s, uint targetMet, Ath__array1D<SEQ*> *residueSeq);
	int computeDiagOU(SEQ *s, uint trackMin, uint trackMax, uint targetMet, Ath__array1D<SEQ*> *residueSeq);
	void printDgContext();
	void initTargetSeq();
	void getDgOverlap(int *options);
	void getDgOverlap(SEQ *sseq, uint dir, Ath__array1D<SEQ*>* dgContext, Ath__array1D<SEQ*>* overlapSeq, Ath__array1D<SEQ*>* residueSeq);

	void writeBoxRaphael3D(FILE *fp, ext2dBox *bb, int *base_ll, int *base_ur, double y1, double th, double volt);
	uint getRSeg(dbNet* net, uint shapeId);

	void allocOUpool();

	int _met;
	int _underMet;
	int _overMet;
	uint _wireCnt;

	uint _dirTable[10000];
	int _minSpaceTable[20];

	int _minWidth;
	int _minSpace;
	int _pitch;

	double _w_m;
	int _w_nm;
	double _s_m;
	int _s_nm;
	double _w2_m;
        int _w2_nm;
        double _s2_m;
        int _s2_nm;

	double _r;
	double _t;
	double _h;

	uint _wIndex;
	uint _sIndex;
	uint _dwIndex;
	uint _dsIndex;
	uint _rIndex;
	uint _pIndex;

	double _topWidth;
	double _botWidth;
	double _teff;
	double _heff;
	double _seff;

	double _topWidthR;
	double _botWidthR;
	double _teffR;
	double _peffR;

	bool _benchFlag;
	bool _varFlag;
	bool _3dFlag;
	bool _over;
	bool _overUnder;
	bool _diag;
	bool _verticalDiag;
	bool _plate;
	bool _thickVarFlag;
	bool _metExtFlag;
	uint _diagModel;

	extDistRC* _rc[20];
	extDistRC* _tmpRC;
	bool _rcValid;
	extRCTable *_capTable;
	Ath__array1D<double> _widthTable;
	Ath__array1D<double> _spaceTable;
	Ath__array1D<double> _dataTable;
	Ath__array1D<double> _pTable;
	Ath__array1D<double> _widthTable0;
	Ath__array1D<double> _spaceTable0;
	Ath__array1D<double> _diagSpaceTable0;
	Ath__array1D<double> _diagWidthTable0;

	Ath__array1D<SEQ*>*** _dgContextArray; // array
	uint *_dgContextDepth; // not array
	uint *_dgContextPlanes; // not array
	uint *_dgContextTracks; // not array
	uint *_dgContextBaseLvl; // not array
	int *_dgContextLowLvl; // not array
	int *_dgContextHiLvl; // not array
	uint *_dgContextBaseTrack; // array
	int *_dgContextLowTrack; // array
	int *_dgContextHiTrack; // array
	int **_dgContextTrackBase; // array
	FILE *_dgContextFile;
	uint _dgContextCnt;

	uint *_ccContextLength;
	Ath__array1D<int> **_ccContextArray;

	Ath__array1D<ext2dBox*> _2dBoxTable[2][20]; // assume 20 layers; 0=main net; 1=context
	AthPool<ext2dBox> *_2dBoxPool;
	uint *_ccMergedContextLength;
	Ath__array1D<int> **_ccMergedContextArray;

	int _ll[2];
	int _ur[2];

	uint _len;
	int _dist;
	uint _width;
	uint _dir;
	uint _layerCnt;
	dbBlock *_block;
	dbTech *_tech;
	double _capMatrix[100][100];
	uint _idTable[10000];
	uint _mapTable[10000];
	uint _maxCapNodeCnt;

	extMain *_extMain;
	extRCModel* _currentModel;
	Ath__array1D<extMetRCTable*> _metRCTable;
	uint _minModelIndex;
	uint _maxModelIndex;

	uint _totCCcnt;	
	uint _totSmallCCcnt;	
	uint _totBigCCcnt;	
	uint _totSignalSegCnt;
	uint _totSegCnt;

	double _resFactor;
	bool _resModify;
	double _ccFactor;
	bool _ccModify;
	double _gndcFactor;
	bool _gndcModify;

	gs *_pixelTable;
	uint **_ouPixelTableIndexMap;

	Ath__array1D<SEQ*> *_diagTable;
	Ath__array1D<SEQ*> *_tmpSrcTable;
	Ath__array1D<SEQ*> *_tmpDstTable;
	Ath__array1D<SEQ*> *_tmpTable;
	Ath__array1D<SEQ*> *_underTable;
	Ath__array1D<SEQ*> *_ouTable;
	Ath__array1D<SEQ*> *_overTable;

	uint _netId;
	int _rsegSrcId;
	int _rsegTgtId;
	int _netSrcId;
	int _netTgtId;
	FILE *_debugFP;

	AthPool<SEQ> *_seqPool;

	AthPool<extLenOU> *_lenOUPool;
	Ath__array1D<extLenOU*> *_lenOUtable;

	bool _diagFlow;
	bool _btermThreshold;
	bool _toHi;
	bool _sameNetFlag;

	bool _rotatedGs;

    dbCreateNetUtil _create_net_util;
};
class extWire
{
public:
	uint _netId;
	int _shapeId;
	dbTechLayer *_layer;
};
class extWireBin
{
public:
	uint _dir;
	uint _num;
	int _base;
	Ath__array1D<extWire*> *_table;
	AthPool<extWire> *_extWirePool;

	extWireBin(uint d, uint num, int base, AthPool<extWire> *wpool, uint allocChunk);
	int addWire(uint netId, int sid, dbTechLayer *layer);
	uint createDbNets(dbBlock *block, dbCreateNetUtil *createDbNet);
	uint createDbNetsGS(dbBlock *block, dbCreateNetUtil *createDbNet);

};


class extTileSystem
{
public:
    Ath__array1D<uint> **_signalTable[2];
    Ath__array1D<uint> **_instTable[2];
    Ath__array1D<uint> *_powerTable;
    Ath__array1D<uint> *_tmpIdTable;
    uint _tileCnt[2];
    uint _tileSize[2];
    int _ll[2];
    int _ur[2];

	extTileSystem(adsRect & extRect, uint *size);
	~extTileSystem();
};


class extWindow
{
public:
	uint _num;
	uint *_dirTable;
	uint *_minSpaceTable;
	int _ll[2];
	int _ur[2];

	uint _maxLayerCnt; // for allocation

	uint _step_nm[2];

	uint _ccDist;

	uint _sigtype;
	uint _pwrtype;

	uint *_pitchTable;
	uint *_widthTable;;
	uint _layerCnt;;

	uint _maxPitch;
	uint _maxWidth;

	int _lo_gs[2];
	int _hi_gs[2];
	int _lo_sdb[2];
	int _hi_sdb[2];
	int _minExtracted;
	int _extractLimit;
	int _gs_limit;
	int _hiXY;
	int _deallocLimit;
	
	int *_extTrack[2];
	int *_extLimit[2];
	int *_cntxTrack[2];
	int *_cntxLimit[2];
	int *_sdbBase[2];

	uint _totPowerWireCnt;
	uint _totSignalWireCnt;
	uint _totWireCnt;
	uint _processPowerWireCnt;
	uint _processSignalWireCnt;
	uint _totalWiresExtracted;


	uint _currentDir;
	bool _gsRotatedFlag;

	extWindow(uint maxLayerCnt);
	void init(uint maxLayerCnt);
	extWindow(extWindow *e, uint num);
	~extWindow();
	
	void initWindowStep(adsRect & extRect, uint trackStep, uint layerCnt, uint modelLevelCnt);
	void makeSdbBuckets(uint sdbBucketCnt[2], uint sdbBucketSize[2], int sdbTable_ll[2], int sdbTable_ur[2]);
	int setExtBoundaries(uint dir);
	int adjust_hiXY(int hiXY);
	int set_extractLimit();
	void reportProcessedWires(bool rlog);
	int getDeallocLimit();
	void updateLoBounds(bool reportFlag);
	void printBoundaries(FILE *fp, bool flag);
	void get_extractLimit(extWindow *e);
	void updateExtLimits(int **limitArray);
	void printExtLimits(FILE *fp);
	void printLimits(FILE *fp, char *header, uint maxLayer, int **limitArray, int **trackArray);
	dbBlock *createExtBlock(extMeasure *m, dbBlock *mainBlock, adsRect & extRect);
	int getIntProperty(dbBlock *block, const char *name);
	void getExtProperties(dbBlock *block);
	void makeIntArrayProperty(dbBlock *block, uint ii, int *A, char *name);
	int getIntArrayProperty(dbBlock *block, uint ii, char *name);
};

class extMainOptions
{
public:

	extMainOptions();

	uint _overDist;
	uint _underDist;
	int _met;
	int _underMet;
	int _overMet;
	uint _wireCnt;
	const char *_topDir;
	const char *_name;
	const char *_wTable;
	const char *_sTable;
	const char *_thTable;
	const char *_dTable;
	
	bool _write_to_solver;
	bool _read_from_solver; 
	bool _run_solver;


	bool _listsFlag;
	bool _thListFlag;
	bool _wsListFlag;
	bool _default_lef_rules;
	bool _nondefault_lef_rules;

	bool _multiple_widths;

	bool _varFlag;
	bool _3dFlag;
	bool _over;
	bool _overUnder;
	bool _diag;

	dbTech *_tech;
	dbBlock *_block;

	Ath__array1D<double> _widthTable;
	Ath__array1D<double> _spaceTable;
	Ath__array1D<double> _densityTable;
	Ath__array1D<double> _thicknessTable;
	Ath__array1D<double> _gridTable;

	int _ll[2];
	int _ur[2];
	uint _len;
	int _dist;
	uint _width;
	uint _dir;
	extRCModel* _rcModel;
	uint _layerCnt;
};

class extCorner
{
	public:

	char *_name;
	int _model;
	int _dbIndex;
	int _scaledCornerIdx;
	float _resFactor;
	float _ccFactor;
	float _gndFactor;
	extCorner *_extCornerPtr;

	extCorner();
};

class extMain
{
private:
	//bool _ouReadReverse;
	bool _batchScaleExt;
	Ath__array1D<extCorner*> *_processCornerTable;
	Ath__array1D<extCorner*> *_scaledCornerTable;

	Ath__array1D<extRCModel*> *_modelTable;
	Ath__array1D<uint> _modelMap; // TO_TEST
	Ath__array1D<extMetRCTable*> _metRCTable;
	double _resistanceTable[20][20];
	double _capacitanceTable[20][20]; // 20 layers by 20 rc models
	double _minWidthTable[20];
	double _tmpCapTable[20];
	double _tmpSumCapTable[20];
	double _tmpResTable[20];
	double _tmpSumResTable[20];
	int _sumUpdated;
	int _minModelIndex; // TO_TEST
	int _typModelIndex; //
	int _maxModelIndex; //

	dbDatabase *_db;
	dbTech *_tech;
	dbBlock *_block;
	uint _blockId;
	extSpef *_spef;
	bool _writeNameMap;
	bool _fullIncrSpef;
	bool _noFullIncrSpef;
	char *_origSpefFilePrefix;
	char *_newSpefFilePrefix;
	char *_excludeCells;
	uint _bufSpefCnt;
	bool _incrNoBackSlash;
	bool _incrSpefPrimtime;
	bool _incrSpefPsta;
	uint _cornerCnt;
	uint _extDbCnt;

	int _remote;
	bool _extracted;
	bool _reExtract;
	bool _allNet;
	bool _eco;
	adsRect *_ibox;

	bool _getBandWire;
	bool _printBandInfo;
	bool _reuseMetalFill;
	uint _ccPreseveGeom;
	uint _ccUp;
	uint _couplingFlag;
	uint _cc_band_tracks;
	uint _use_signal_tables; 
				// 1: signal table,  NO rotation; 
				// 2: signal table,  rotation; 
				// 3: NO signal table,  rotation
	bool _rotatedGs;
	uint _ccContextDepth;
	uint _debug;
	int _ccMinX;
	int _ccMinY;
	int _ccMaxX;
	int _ccMaxY;
	double _mergeResBound;
	bool _mergeViaRes;
	bool _mergeParallelCC;
	bool _unifiedMeasureInit;
	bool _reportNetNoWire;
	int _netNoWireCnt;

	double _resFactor;
	bool _resModify;
	double _ccFactor;
	bool _ccModify;
	double _gndcFactor;
	bool _gndcModify;

	float _netGndcCalibFactor;
	bool _netGndcCalibration;

	bool _useDbSdb;

	Ath__array1D<int> *_nodeTable;
	Ath__array1D<int> *_btermTable;
	Ath__array1D<int> *_itermTable;
	
	ZPtr<ISdb> _extNetSDB;
	ZPtr<ISdb> _extCcapSDB;
	ZPtr<ISdb> _reExtCcapSDB;
	ZPtr<IZdcr> _extDcr;
	ZPtr<IZgui> _extGui;
	uint _menuId;
	uint _dbPowerId;
	uint _dbSignalId;
	uint _RsegId;
	uint _CCsegId;
	
	uint _CC_menu_self_id;
	uint _CC_menu_net1_id;
	uint _CC_menu_net2_id;

	uint _CCnoPowerSource;
	uint _CCnoPowerTarget;
	int _x1;
	int _y1; 
	int _x2;
	int _y2;

	double _coupleThreshold;

	uint _totCCcnt;	
	uint _totSmallCCcnt;	
	uint _totBigCCcnt;	
	uint _totSignalSegCnt;
	uint _totSegCnt;

	bool _lefRC;
	bool _noModelRC;
	extRCModel* _currentModel;

	uint *_singlePlaneLayerMap;
	uint **_overUnderPlaneLayerMap;
	bool _usingMetalPlanes;
	bool _alwaysNewGs;

	gs *_geomSeq;

	AthPool<SEQ> *_seqPool;

	Ath__array1D<SEQ*>*** _dgContextArray;
	uint _dgContextDepth;
	uint _dgContextPlanes;
	uint _dgContextTracks;
	uint _dgContextBaseLvl;
	int _dgContextLowLvl;
	int _dgContextHiLvl;
	uint *_dgContextBaseTrack;
	int *_dgContextLowTrack;
	int *_dgContextHiTrack;
	int **_dgContextTrackBase;

	uint *_ccContextLength;
	Ath__array1D<int> **_ccContextArray;
	uint *_ccMergedContextLength;
	Ath__array1D<int> **_ccMergedContextArray;
	Ath__array1D<int> *_tContextArray;

	extGeoThickTable **_geoThickTable;
	int _measureRcCnt;
	int _shapeRcCnt;
	int _updateTotalCcnt;
	FILE *_printFile;
	FILE *_ptFile;

	uint _extRun;
	dbExtControl *_prevControl;

	bool _independentExtCorners;
	bool _foreign;
	bool _rsegCoord;
	bool _overCell;
	bool _diagFlow;

	std::vector<uint> _rsegJid;
	std::vector<uint> _shortSrcJid;
	std::vector<uint> _shortTgtJid;

	std::vector<dbBTerm *>  _connectedBTerm;
	std::vector<dbITerm *>  _connectedITerm;

	std::vector<dbNet *> _dumNet;
	std::vector<dbInst *> _dumInst;
	std::vector<dbCapNode *> _dumCap;
	std::vector<dbCCSeg *> _dumCC;
	std::vector<dbRSeg *> _dumRSeg;
	std::vector<int> _dumInt;

	Ath__gridTable *_search;

	int _noVariationIndex;

	extWireBin ***_wireBinTable;
	extWireBin ***_cntxBinTable;
	Ath__array1D<uint> ***_cntxInstTable;


	extTileSystem *_tiles;
	bool _ignoreWarning_1st;
	bool _keepExtModel;

	FILE *_searchFP;
	friend class extMeasure;

	// 021411D BEGIN
        FILE *_blkInfoVDD;
        FILE *_viaInfoVDD;
        FILE *_blkInfoGND;
        FILE *_viaInfoGND;

        FILE *_stdCirVDD;
        FILE *_globCirVDD;
        FILE *_globGeomVDD;
        FILE *_stdCirGND;
        FILE *_globCirGND;

        FILE *_stdCirHeadVDD;
        FILE *_globCirHeadVDD;
        FILE *_globGeomGND;
        FILE *_stdCirHeadGND;
        FILE *_globCirHeadGND;
	FILE *_blkInfo;
	FILE *_viaInfo;
	FILE *_globCir;
	FILE *_globGeom;
	FILE *_stdCir;
	FILE *_globCirHead;
	FILE *_stdCirHead;
	FILE *_viaStackGlobCir;
	FILE *_viaStackGlobVDD;
	FILE *_viaStackGlobGND;
	// 021411D END

	// 021511D BEGIN
	Ath__array1D<int> *_junct2viaMap;
	bool _dbgPowerFlow;
	dbCreateNetUtil *_netUtil;
	// 021511D END

	// 021911D BEGIN
	std::vector<dbBox*> _viaUp_VDDtable;
	std::vector<dbBox*> _viaUp_GNDtable;
	std::vector<dbBox*> _viaM1_GNDtable;
	std::vector<dbBox*> _viaM1_VDDtable;
	std::vector<dbBox*> *_viaM1Table;
	std::vector<dbBox*> *_viaUpTable;
	// 021911D END
	bool _adjust_colinear;
	// 032811D END
	// 061711D BEGIN
	uint _stackedViaResCnt;
	uint _totViaResCnt;
	Ath__array1D<int> *_via2JunctionMap;
	std::map<dbBox *, dbNet *>   _via_map;
	std::map<uint, dbNet *>   _via_id_map;
	std::map<uint, float>   _capNode_map;
	std::vector<dbInst*> _powerMacroTable;
	std::vector<dbBox*> _viaUpperTable[2];
	// 061711D END
	// 021712D BEGIN
	Ath__array1D<char*> **_supplyViaMap[2];
	Ath__array1D<dbBox *> **_supplyViaTable[2];
	char * _power_source_file;
	std::vector<char*> _powerSourceTable[2];
	// 021712D END
	FILE *_coordsFP;
	FILE *_coordsGND;
	FILE *_coordsVDD;
	std::vector<uint> _vddItermIdTable;
	std::vector<uint> _gndItermIdTable;
        FILE *_subCktNodeFP[2][2];
        uint _subCktNodeCnt[2][2];
	bool _power_extract_only;
	bool _skip_power_stubs;
	bool _skip_m1_caps;
	const char *_power_exclude_cell_list;
	//062212D
	bool _nodeCoords;
	int _prevX;
	int _prevY;
	char _node_blk_dir[1024];
	char _node_blk_prefix[1024];
        char _node_inst_prefix[1024];
	//100812D
	Ath__array1D<dbITerm*> *_junct2iterm;
	//102912D
	std::map<uint, dbSBox *>   _sbox_id_map;

	uint _powerWireCnt;
	uint _mergedPowerWireCnt;
	uint _overlapPowerWireCnt;
	uint _viaOverlapPowerCnt;
	uint _multiViaCnt;

	std::vector<adsRect *> _multiViaTable[20];
	std::vector<dbBox *> _multiViaBoxTable[20];

public:

	enum INCR_SPEF_TYPE
	{
		ISPEF_NONE,
		ISPEF_ORIGINAL,
		ISPEF_NEW,
		ISPEF_ORIGINAL_PLUS_HALO,
		ISPEF_NEW_PLUS_HALO,
	} ;
	extMain(uint menuId);

    uint getDir(int x1, int y1, int x2, int y2);
	uint getDir(adsRect &r);
	bool outOfBounds_i(int limit[2], int v);
	bool outOfBounds_d(double limit[2], double v);
	bool printNetRC(char *buff, dbNet *net, extNetStats *st);
	bool printNetDB(char *buff, dbNet *net, extNetStats *st);
	uint printNetStats(FILE *fp, dbBlock *block, extNetStats *st, bool skipRC, bool skipDb, bool skipPower, std::list<int> *list_of_nets);

	uint initSearchForNets(int *X1, int *Y1, uint *pitchTable, uint *widthTable, uint *dirTable, adsRect & extRect, bool skipBaseCalc);
	uint addNetSBoxes(dbNet *net, uint dir, int *bb_ll, int *bb_ur, uint wtype, dbCreateNetUtil *netUtil=NULL);
	uint addNetSBoxes2(dbNet *net, uint dir, int *bb_ll, int *bb_ur, uint wtype, uint step=0);
	uint addPowerNets(uint dir, int *bb_ll, int *bb_ur, uint wtype, dbCreateNetUtil *netUtil=NULL);
	uint addNetShapesOnSearch(dbNet * net, uint dir, int *bb_ll, int *bb_ur, uint wtype, FILE *fp, dbCreateNetUtil *netUtil=NULL);
	uint addSignalNets(uint dir, int *bb_ll, int *bb_ur, uint wtype, dbCreateNetUtil *netUtil=NULL);
	uint addInstsGs(Ath__array1D<uint> *instTable, Ath__array1D<uint> *tmpInstIdTable, uint dir);
	uint mkSignalTables(uint *nm_step, int *bb_ll, int *bb_ur, Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> ***signalGsTable, uint *bucketCnt);
	uint mkSignalTables(uint *nm_step, int *bb_ll, int *bb_ur, Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> ***signalGsTable, Ath__array1D<uint> ***instTable, uint *bucketCnt);
	void freeSignalTables(bool rlog, Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> ***signalGsTable, uint *bucketCnt);
	uint addNets(uint dir, int *bb_ll, int *bb_ur, uint wtype, uint ptype, Ath__array1D<uint> *sdbSignalTable);
	uint addNetOnTable(uint netId, uint dir, adsRect *maxRect, uint *nm_step, int *bb_ll, int *bb_ur, Ath__array1D<uint> ***wireTable);
	void getNetShapes(dbNet *net, adsRect **maxRectSdb, adsRect &maxRectGs, bool *hasSdbWires, bool &  hasGsWires);
	void getNetSboxes(dbNet *net, adsRect **maxRectSdb, adsRect &maxRectGs, bool *hasSdbWires, bool &  hasGsWires);
	uint addNetsGs(Ath__array1D<uint> *gsTable, int dir);
	uint addNetShapesGs(dbNet * net, bool gsRotated, bool swap_coords, int dir, dbCreateNetUtil *createDbNet=NULL);
	uint addNetSboxesGs(dbNet * net, bool gsRotated, bool swap_coords, int dir, dbCreateNetUtil *createDbNet=NULL);

	uint getBucketNum(int base, int max, uint step, int xy);
	int getXY_gs(int base, int XY, uint minRes);
	int getXY_gs2(int base, int hiXY, int loXY, uint minRes);
	int fill_gs(int dir, int *ll, int *ur, int hiXY, int minExt, uint minRes, uint layerCnt, uint *pitchTable, uint *widthTable);
	int fill_gs2(int dir, int *ll, int *ur, int *lo_gs, int *lo_gst, uint layerCnt, uint *dirTable, uint *pitchTable, uint *widthTable, uint bucket, Ath__array1D<uint> ***gsTable, Ath__array1D<uint> ***instGsTable);
	//int fill_gs2(int dir, int *ll, int *ur, int hiXY, int minExt, uint layerCnt, uint *dirTable, uint *pitchTable, uint *widthTable);
	uint couplingFlow(bool rlog, adsRect & extRect, uint trackStep, uint ccDist, extMeasure *m, void (*coupleAndCompute)(int *, void *));
	uint initPlanes(uint dir, uint layerCnt, uint *pitchTable, uint *widthTable, int *ll, int *ur);
	uint initPlanes(uint dir, int *wLL, int *wUR, uint layerCnt, uint *pitchTable, uint *widthTable, uint *dirTable, int *bb_ll, bool skipMemAlloc=false);

	uint couplingWindowFlow(bool rlog, adsRect & extRect, uint trackStep, uint ccFlag, bool doExt, extMeasure *m, void (*coupleAndCompute)(int *, void *));
	bool isIncluded(adsRect & r, uint dir, int *ll, int *ur);
	bool matchDir(uint dir, adsRect & r);
	bool isIncludedInsearch(adsRect & r, uint dir, int *bb_ll, int *bb_ur);

	uint makeTree(uint netId);
	uint benchNets(extMainOptions *opt, uint netId, uint trackCnt, ZPtr<ISdb> netSdb);
	uint runSolver(extMainOptions *opt, uint netId, int shapeId);

    bool printNetStats(FILE *fp, dbBlock *block, bool skipRC, bool skipDb, bool skipPOwer);
	void resetSumRCtable();
	void addToSumRCtable();
	void copyToSumRCtable();
	uint getResCapTable(bool lefRC);
	double getLoCoupling();
	void ccReportProgress();
	void measureRC(int *options);
	void updateTotalRes(dbRSeg *rseg1, dbRSeg *rseg2, extMeasure *m, double *delta, uint modelCnt);
	void updateTotalCap(dbRSeg *rseg, double frCap, double ccCap, double deltaFr, uint modelIndex);
	void updateTotalCap(dbRSeg *rseg, extMeasure *m, double *deltaFr, uint modelCnt, bool includeCoupling, bool includeDiag=false);
	void updateCCCap(dbRSeg *rseg1, dbRSeg *rseg2, double ccCap);
	double measureOverUnderCap(extMeasure *m, int x1, int y1, int x2, int y2);
	uint initPlanes(uint layerCnt, adsRect *bb=NULL);
	uint allocateOverUnderMaps(uint layerCnt);
	uint initPlanesOld(uint cnt);
	uint initPlanesNew(uint cnt, adsRect *bb=NULL);
	uint makeIntersectPlanes(uint layerCnt);
	void deletePlanes(uint layerCnt);
	void getBboxPerLayer(adsRect *rectTable);
	
	uint readCmpStats(const char* name, uint &tileSze, int &X1, int &Y1, int &X2, int &Y2);
	uint readCmpFile(const char* name);
	int setMinTypMax(bool minModel, bool typModel, bool maxModel, 
		const char *cmp_file, bool density_model, bool litho, 
		int setMin, int setTyp, int setMax, uint cornerCnt);

	extRCModel* getRCmodel(uint n);

	double getLefResistance(uint level, uint width, uint length, uint model);
	double getResistance(uint level, uint width, uint len, uint model);
	double getFringe(uint met, uint width, uint modelIndex, double &areaCap);
	void printNet(dbNet *net, uint netId);
	double calcFringe(extDistRC *rc, double deltaFr, bool includeCoupling);
	double updateTotalCap(dbRSeg *rseg, double cap, uint modelIndex);
	double updateRes(dbRSeg *rseg, double res, uint model);

	//void extCompute(void *a, void *b, int c);

	void setCrawler(ZPtr<IZdcr> z, uint id);
	void setIgui(ZPtr<IZgui> z);
	uint getExtBoxes(ZPtr<IZdcr> dcr, uint boxType);
	uint extInspect(ZPtr<IZdcr> dcr, uint boxType);
	uint makeGuiBoxes(uint extGuiBoxType);
	uint setupSearchDb(const char *bbox, uint debug, ZInterface *Interface);
	ZPtr<ISdb> getCcSdb();
	ZPtr<ISdb> getNetSdb();
	uint computeXcaps(uint boxType);
	bool getExtAreaCoords(const char *bbox);
	uint getExtBbox(int *x1, int *y1, int *x2, int *y2);
	void setExtractionBbox(const char *bbox);
	
	void getSearchDb()
	{
		_extNetSDB= _block->getSearchDbNets();
	}
	void setupMapping(uint itermCnt);
	uint getMultiples(uint cnt, uint base);
	uint getExtLayerCnt(ads::dbTech *tech);
	uint addExtModel(dbTech *tech=NULL);
	uint readExtRules(const char *name, const char *filename, int min, int typ, int max);

	void setDB(dbDatabase *db);
	void setBlock(dbBlock *block);
    	dbBlock *getBlock() { return _block; }
    	dbTech *getTech() { return _tech; }
	extRCModel *getRCModel() { return _modelTable->get(0); }

//	int db_test_wires();

	int db_test_wires( dbDatabase * db=NULL );

	void print_RC(dbRSeg *rc);
	void resetMapping(dbBTerm *term, dbITerm *iterm, uint junction);
	uint resetMapNodes(dbNet *net);
	void setResCapFromLef(dbRSeg *rc, uint targetCapId, dbShape & s, uint len);
	bool isTermPathEnded (dbBTerm *bterm, dbITerm *iterm);
	uint getCapNodeId(dbWirePath & path, dbNet *net, bool branch);
	uint getCapNodeId(dbNet *net, dbBTerm *bterm, dbITerm *iterm, uint junction, bool branch=false);
	void unlinkExt(std::vector<dbNet *> & nets);
	void unlinkCC(std::vector<dbNet *> & nets);
	void unlinkRSeg(std::vector<dbNet *> & nets);
	void unlinkCapNode(std::vector<dbNet *> & nets);
	void removeExt(std::vector<dbNet *> & nets);
	void removeExt();
	void removeCC(std::vector<dbNet *> & nets);
	void removeRSeg(std::vector<dbNet *> & nets);
	void removeCapNode(std::vector<dbNet *> & nets);
	void removeSdb(std::vector<dbNet *> & nets);
	void adjustRC(double resFactor, double ccFactor, double gndcFactor);
	void updatePrevControl();
	void getPrevControl();
	
	uint makeBlockRCsegs(bool btermThresholdFlag, const char *cmp, bool density_model, bool litho, 
		const char *netNames, const char *bbox, const char *ibox, uint cc_up, uint ccFlag, int ccBandTracks, 
		uint use_signal_table, double resBound, bool mergeViaRes, uint debug, 
		int cc_preseve_geom, bool re_extract, bool eco, bool gs, bool log, ZPtr<ISdb> sdb, 
		double ccThres, int contextDepth, bool overCell, const char *extRules, ZInterface *context);

	uint getShortSrcJid(uint jid);
	void markPathHeadTerm(dbWirePath & path);
	void make1stRSeg(dbNet *net, dbWirePath & path, uint cnid, bool skipStartWarning);
	uint makeNetRCsegs_old(dbNet *net, double resBound, uint debug=0);
	uint makeNetRCsegs(dbNet *net, bool skipStartWarning=false);
	uint addPowerGs(int dir=-1, int *ll=NULL, int *ur=NULL);
	uint addSignalGs(int dir=-1, int *ll=NULL, int *ur=NULL);
	uint addItermShapesOnPlanes(dbInst *inst, bool rotatedFlag, bool swap_coords);
	uint addObsShapesOnPlanes(dbInst *inst, bool rotatedFlag, bool swap_coords);
	uint addInstShapesOnPlanes(uint dir=0, int *ll=NULL, int *ur=NULL);

	void getShapeRC(dbNet *net, dbShape & s, adsPoint & prevPoint, dbWirePathShape & pshape);
	void setResAndCap(dbRSeg *rc, double *restbl, double *captbl);
	void setBranchCapNodeId(dbNet *net, uint junction);
	void addRSeg(dbNet *net,  std::vector<uint> & rsegJid, uint &srcId, adsPoint &prevPoint, dbWirePath &path, dbWirePathShape &pshape, bool isBranch, double *restbl, double *captbl);
	uint print_shape( dbShape & shape, uint j1, uint j2);
	uint getNodeId(dbWirePath & path, bool branch, uint *nodeType);
	uint getNodeId(dbWirePathShape & pshape, uint *nodeType);
	uint computePathDir(adsPoint & p1, adsPoint & p2, uint *length);
	uint openSpefFile(char *filename, uint mode);

	//-------------------------------------------------------------- SPEF
	uint match(char *filename, bool m_map, const char *dbCornerName, int corner, int spefCorner);
	uint calibrate(char *filename, bool m_map, float upperLimit, float lowerLimit, const char *dbCornerName, int corner, int spefCorner);
	void setUniqueExttreeCorner();
	uint readSPEF(char *filename, char *netNames, bool force, bool useIds, bool rConn, char *nodeCoord, bool rCap, bool rOnlyCCcap, bool rRes, float cc_thres, float cc_gnd_factor, float length_unit, bool m_map, bool noCapNumCollapse, char *capNodeMapFile, bool log, int corner, double low, double up, char *excludeSubWord, char *subWord, char *statsFile, const char *dbCornerName, const char *calibrateBaseCorner, int spefCorner, int fixLoop, bool keepLoadedCorner, bool stampWire=false, ZPtr<ISdb> netSdb=NULL,  uint testParsing=0, bool moreToRead=false, bool diff=false, bool calib=false, int app_ptint_limit=0);
	uint readSPEFincr(char *filename);
	uint writeSPEF(bool stop);
	uint writeSPEF(uint netId, bool single_pi, uint debug, int corner, const char *corner_name);
	uint writeSPEF(char *filename, char *netNames, bool useIds, bool noNameMap, char *nodeCoord, bool termJxy, const char* excludeCells, const char* capUnit, const char* resUnit, bool gzFlag, bool stopAfterMap, bool wClock, bool wConn, bool wCap, bool wOnlyCCcap, bool wRes, bool noCnum, bool initOnly, bool single_pi, bool no_backslash, bool prime_time, bool psta,
		int corner, const char *corner_name, bool flatten, bool parallel);
	uint writeNetSPEF(dbNet *net, double resBound, uint debug);
	uint makeITermCapNode(uint id, dbNet *net);
	uint makeBTermCapNode(uint id, dbNet *net);

	void initIncrementalSpef(const char *origp, const char *newp, const char *reader, const char *excludeCell, bool noBackSlash);
	void reportTotalCap(const char *file,bool cap,bool res,double ccmult,const char *ref,const char *rd_file);
	void reportTotalCc(const char *file,const char *ref,const char *rd_file);
	double getTotalNetCap(uint netId, uint cornerNum);
	void extDump(char *filename, bool openTreeFile, bool closeTreeFile, bool ccCapGeom, bool ccNetGeom, bool trackCnt, bool signal, bool power, uint layer);
	void extCount(bool signalWireSeg, bool powerWireSeg);
	void initContextArray();
	void initDgContextArray();
	void removeDgContextArray();

	//ruLESgeN
	bool getFirstShape(dbNet *net, dbShape & shape);
	uint readProcess(const char *name, const char *file);
	uint rulesGen(const char *name, const char *topDir, const char *rulesFile, int pattern, bool skipSolv, bool readSolv, bool runSolver, bool keepFile);
	uint metRulesGen(const char *name, const char *topDir, const char *rulesFile, int pattern, bool writeFiles, bool readFiles, bool runSolver, bool keepFile, uint met);
	uint writeRules(const char *name, const char *topDir, const char *rulesFile, int pattern, bool readFiles = false);
	uint benchWires(extMainOptions *options);
	FILE *getPtFile () { return _ptFile; };
	static void destroyExtSdb (std::vector<dbNet *> & nets, void *ext);
	void writeIncrementalSpef (char *filename, std::vector<dbNet *> & buf_nets, uint nn, bool dual_incr_spef);
	void writeIncrementalSpef (std::vector<dbNet *> & buf_nets, INCR_SPEF_TYPE type, bool coupled_rc, bool dual_incr_spef);
	static void writeIncrementalSpef (Darr<dbNet*> & buf_nets, dbBlock *block, INCR_SPEF_TYPE type, bool coupled_rc, bool dual_incr_spef);
	static void writeIncrementalSpef (Darr<dbNet*> & buf_nets, std::vector<dbNet*> &ccHaloNets, dbBlock *block, INCR_SPEF_TYPE type, bool coupled_rc, bool dual_incr_spef);
	static void writeIncrementalSpef (std::vector<dbNet *> & buf_nets, dbBlock *block, INCR_SPEF_TYPE type, bool coupled_rc, bool dual_incr_spef);
	static void writeIncrementalSpef (std::vector<dbNet *> & buf_nets, std::vector<dbNet*> &ccHaloNets, dbBlock *block, INCR_SPEF_TYPE type, bool coupled_rc, bool dual_incr_spef);
	void writeSpef (char *filename, std::vector<dbNet *> & tnets, int corner, char *coord, char *reader);
	static int getExtCornerIndex (dbBlock *block, const char *cornerName);

	static void initExtractedCorners(dbBlock *block);

	void addDummyCorners(uint cornerCnt);
	static void addDummyCorners(dbBlock *block, uint cnt);
	char *addRCCorner(const char *name, int model, int userDefined=1);
	char *addRCCornerScaled(const char *name, uint model, float resFactor, float ccFactor, float gndFactor);
	void getCorners(std::list<std::string>&ecl);
	void deleteCorners();
	void cleanCornerTables();
	int getDbCornerIndex(const char *name);
	int getDbCornerModel(const char *name);
	bool setCorners(const char *rulesFileName, const char *cmp_file);
	int getProcessCornerDbIndex (int pcidx);
	void getScaledCornerDbIndex (int pcidx, int & sidx, int & scdbIdx);
	void getScaledRC (int sidx, double & res, double & cap);
	void getScaledGndC (int sidx, double & cap);
	void getScaledCC (int sidx, double & cap);
	void genScaledExt();
	// void makeCornerNameMap(char *buff, int cornerCnt, bool spef);
	void makeCornerNameMap();
	void getExtractedCorners();
	void makeCornerMapFromExtControl();
	bool checkLayerResistance();

	uint getNetBbox(dbNet *net, adsRect &maxRect);
	uint mkSignalTables2(uint *nm_step, int *bb_ll, int *bb_ur, Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> *sdbPowerTable, 
							 Ath__array1D<uint> ***instTable, uint *bucketCnt);
	uint addSignalNets2(uint dir, int *lo_sdb, int *hi_sdb, int *bb_ll, int *bb_ur, uint *bucketSize, uint wtype,
		Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> *tmpNetIdTable, dbCreateNetUtil *createDbNet=NULL);

	uint addPowerNets2(uint dir, int *bb_ll, int *bb_ur, uint wtype, Ath__array1D<uint> *sdbPowerTable, dbCreateNetUtil *createDbNet=NULL);
	void resetNetSpefFlag(Ath__array1D<uint> *tmpNetIdTable);
	
	int fill_gs3(int dir, int *ll, int *ur, int* lo_gs, int* hi_gs, 
					  uint layerCnt, uint *dirTable, uint *pitchTable, uint *widthTable, 
					  int *sdbTable_ll, int *sdbTable_ur, uint *bucketSize, 
					  Ath__array1D<uint> *powerNetTable, Ath__array1D<uint> *tmpNetIdTable,
					  Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> ***instGsTable,
					  dbCreateNetUtil *createDbNet=NULL);

	void reportTableNetCnt(uint *sdbBucketCnt, Ath__array1D<uint> ***sdbSignalTable);
	uint sBoxCounter(dbNet *net, uint &maxWidth);
	uint powerWireCounter(uint &maxWidth);
	uint signalWireCounter(uint &maxWidth);
	bool getRotatedFlag();
	bool enableRotatedFlag();
	uint addShapeOnGs(dbShape *s, bool swap_coords);
	uint addSBoxOnGs(dbSBox *s, bool swap_coords);
	
	uint addMultipleRectsOnSearch(adsRect &r, uint level, uint dir, uint id, uint shapeId, uint wtype);
	
	//--------------- Window	
	extWindow *initWindowSearch(adsRect & extRect, uint trackStep, uint ccFlag, uint modelLevelCnt, extMeasure *m);
	void fillWindowGs(extWindow *W, int *sdbTable_ll, int *sdbTable_ur, uint *bucketSize, 
		Ath__array1D<uint> *powerNetTable, Ath__array1D<uint> *tmpNetIdTable,
		Ath__array1D<uint> ***sdbSignalTable, Ath__array1D<uint> ***instGsTable, dbCreateNetUtil *createDbNet=NULL);
	uint fillWindowSearch(extWindow *W, int *lo_sdb, int *hi_sdb, int *sdbTable_ll, int *sdbTable_ur, uint *bucketSize, 
		Ath__array1D<uint> *powerNetTable, Ath__array1D<uint> *tmpNetIdTable,
		Ath__array1D<uint> ***sdbSignalTable, dbCreateNetUtil *createDbNet=NULL);
	uint addShapeOnGS(dbNet * net, uint sId, adsRect &r, bool plane, dbTechLayer *layer, 
		bool gsRotated, bool swap_coords, int dir, bool specialWire=false,dbCreateNetUtil *createDbNet=NULL);
	uint extractWindow(bool rlog, extWindow *W, adsRect & extRect, bool single_sdb, 
		extMeasure *m, void (*coupleAndCompute)(int *, void *),
		int *sdbTable_ll=NULL, int *sdbTable_ur=NULL, uint *bucketSize=NULL, 
		Ath__array1D<uint> *powerNetTable=NULL, Ath__array1D<uint> *tmpNetIdTable=NULL,
		Ath__array1D<uint> ***sdbSignalTable=NULL, Ath__array1D<uint> ***instGsTable=NULL);	
	uint couplingTileFlow(bool rlog, adsRect & extRect, extMeasure *m, void (*coupleAndCompute)(int *, void *));

	uint createWindowsDB(bool rlog, adsRect & extRect, uint trackStep, uint ccFlag, uint use_signal_tables);
	uint fillWindowsDB(bool rlog, adsRect & extRect, uint use_signal_tables);
	uint fill_gs4(int dir, int *ll, int *ur, int* lo_gs, int* hi_gs, 
					  uint layerCnt, uint *dirTable, uint *pitchTable, uint *widthTable, 
					  dbCreateNetUtil *createDbNet);
	
	uint createNetShapePropertires(dbBlock *blk);
	void resetGndCaps();
	uint rcGenTile(dbBlock *blk);
	uint mkTileNets(uint dir, int *lo_sdb, int *hi_sdb, bool powerNets, dbCreateNetUtil *createDbNet, uint &rcCnt, bool countOnly=false);
	uint mkTilePowerNets(uint dir, int *lo_sdb, int *hi_sdb, dbCreateNetUtil *createDbNet);
	uint rcNetGen(dbNet *net);
	uint rcGen(const char *netNames, double resBound, bool mergeViaRes, uint debug, bool rlog,  ZInterface *Interface);

	void disableRotatedFlag();
	FILE *openSearchFile(char *name);
	void closeSearchFile();
	
	void addExtWires(adsRect & r, extWireBin ***wireBinTable, uint netId, int shapeId, dbTechLayer *layer, 
						   uint *nm_step, int *bb_ll, int *bb_ur, AthPool<extWire> *wpool, bool cntxFlag);
	extWireBin ***mkSignalBins(uint binSize, int *bb_ll, int *bb_ur, uint *bucketCnt, AthPool<extWire> *wpool, bool cntxFlag);
	uint addNets3(uint dir, int *lo_sdb, int *hi_sdb, int *bb_ll, int *bb_ur, uint bucketSize,
							 extWireBin ***wireBinTable, dbCreateNetUtil *createDbNet);
	uint addNets3GS(uint dir, int *lo_sdb, int *hi_sdb, int *bb_ll, int *bb_ur, uint bucketSize, 
							 extWireBin ***wireBinTable, dbCreateNetUtil *createDbNet);
	Ath__array1D<uint> ***mkInstBins(uint binSize, int *bb_ll, int *bb_ur, uint *bucketCnt);
	uint addInsts(uint dir, int *lo_gs, int *hi_gs, int *bb_ll, int *bb_ur, uint bucketSize, 
					   Ath__array1D<uint> ***wireBinTable, dbCreateNetUtil *createDbNet);
	
	void printLimitArray(int **limitArray, uint layerCnt);
	uint mkTileBoundaries(bool skipPower, bool skipInsts);
	uint mkNetPropertiesForRsegs(dbBlock *blk, uint dir);
	uint rcGenBlock(dbBlock *block=NULL);
	void writeMapping(dbBlock *block=NULL);
	uint invalidateNonDirShapes(dbBlock *blk, uint dir, bool setMainNet);

	uint getNetBbox(dbNet *net, adsRect *maxRect[2]);

	static dbRSeg *getRseg(dbNet *net, uint shapeId);

	static uint assemblyExt(dbBlock *mainBlock, dbBlock *blk);
	static uint assemblyExt__2(dbBlock *mainBlock, dbBlock *blk);
	static dbNet *getDstNet(dbNet *net, dbBlock *dstBlock, Ath__parser *parser);
	static dbRSeg *getMainRSeg(dbNet *srcNet, int srcShapeId, dbNet *dstNet);
	static dbRSeg *getMainRSeg2(dbNet *srcNet, int srcShapeId, dbNet *dstNet);
	static dbRSeg *getMainRSeg3(dbNet *srcNet, int srcShapeId, dbNet *dstNet);
	static uint assemblyCCs(dbBlock *mainBlock, dbBlock *blk, uint cornerCnt, uint &missCCcnt);
	static dbRSeg *getMainRseg(dbCapNode *node, dbBlock *blk, Ath__parser *parser);
	static void updateRseg(dbRSeg *rc1, dbRSeg *rseg2, uint cornerCnt);
	static uint assembly_RCs(dbBlock *mainBlock, dbBlock *blk, uint cornerCnt);

	// 021710D BEGIN
	uint addRCtoTop(dbBlock *blk, bool write_spef);
	uint createCapNodes(dbNet *net, dbNet *parentNet, uint *capNodeMap, uint baseNum);
	uint createRSegs(dbNet *net, dbNet *parentNet, uint *capNodeMap);
	//uint createCCsegs(dbNet *net, dbNet *parentNet, uint *capNodeMap);
	uint write_spef_nets(bool flatten, bool parallel);
	//uint adjustCapNode(dbNet *net, dbITerm *from_child_iterm, uint node_num);
	extSpef *getSpef();
	static uint printRSegs(dbNet *net);
	// 021710D END

	// 022110D BEGIN
	bool adjustChildNode(dbCapNode *childNode, dbNet *parentNet, uint *capNodeMap);
	bool createParentCapNode(dbCapNode *node, dbNet *parentNet, uint nodeNum, uint *capNodeMap, uint baseNum);
	uint adjustParentNode(dbNet *net, dbITerm *from_child_iterm, uint node_num);
	uint createCCsegs(dbNet *net, dbNet *parentNet, dbNet *topDummyNet, uint *capNodeMap, uint baseNum);
	// 022110D END

	// 022210D BEGIN
	uint markCCsegs(dbBlock *blk, bool flag);
	// 022210D END

	// 022310D BEGIN
	void createTop1stRseg(dbNet *net, dbNet *parentNet);
	// 022310D END

	// 021111D BEGIN
	uint getLayerSearchBoundaries(dbTechLayer * layer, int *xyLo, int *xyHi, uint *pitch);
	void railConn(uint dir, dbTechLayer *layer, dbNet *net, int *xyLo, int *xyHi, uint *pitch);
	void railConn(dbNet *net);
	void railConn2(dbNet *net);
	bool isSignalNet(dbNet *net);
	uint powerRCGen();
	// 021111D END
	// 021311D BEGIN
	uint mergeRails(uint dir, std::vector<dbBox *> &boxTable, std::vector<adsRect *> &mergeTable);
	dbITerm *findConnect(dbInst *inst, dbNet *net, dbNet *targetNet);
	uint getITermConn2(uint dir, dbWireEncoder &encoder, dbWire * wire, dbNet *net, int *xy, int *xy2);
	uint getITermConn(uint dir, dbWireEncoder &encoder, dbWire * wire, dbNet *net, int *xy, int *xy2);
	uint viaAndInstConn(uint dir, uint width, dbTechLayer *layer, dbWireEncoder &encoder, dbWire * wire, dbNet *net, adsRect *w, bool skipSideMetalFlag);
	dbNet *createRailNet(dbNet *pnet, dbTechLayer *layer, adsRect *w);
	//void print_shapes(dbWire * wire);
	uint print_shapes(FILE *fp, dbWire * wire);
	// 021311D END
	// 021411D BEGIN
	FILE *openNanoFile(const char *name, const char *name2, const char *suffix, const char *perms);
	void openNanoFiles();
	void closeNanoFiles();
	void setupNanoFiles(dbNet *net);
	void writeResNode(FILE *fp, dbCapNode *capNode, uint level);
	double writeRes(FILE *fp, dbNet *net, uint level, uint width, uint dir, bool skipFirst);
	// 021411D END
	// 021511D BEGIN
	void writeViaRC(FILE *fp, uint level, dbTechLayer *layer, dbBox *v, dbBox *w);
	uint connectStackedVias(dbNet * net, dbTechLayer *layer, bool mergeViaRes);
	uint via2viaConn(dbNet *net, dbBox *v, dbTechLayer *layer, bool mergeviaRes);
	// 021511D END
	// 021611D BEGIN
	void writeSubckt(FILE *fp, char *keyword, char *vdd, char *std, char *cont);
	void writeCapNodes(FILE *fp, dbNet *net, uint level, bool onlyVias, bool skipFirst);
	void writeCapNodes_0713(FILE *fp,dbNet *net, uint level, bool onlyVias);
	// 021611D END
	// 021811D BEGIN
	bool specialMasterType(dbInst *inst);
	// 021811D END
	// 021911D BEGIN
	uint iterm2Vias(dbInst *inst, dbNet *net);
	uint getPowerNets(std::vector<dbNet*> &powerNetTable);
	float getPowerViaRes(dbBox *v, float val);
	uint findHighLevelPinMacros(std::vector<dbInst*> &instTable);
	uint writeViaInfo(FILE *fp, std::vector<dbBox*> &viaTable, bool m1Vias, bool power);
	// 021911D END

	void set_adjust_colinear(bool v);
	// 032811D END
	uint writeViaInfo_old(FILE *fp, std::vector<dbBox*> &viaTable, bool m1Vias);
	uint writeViaCoords(FILE *fp, std::vector<dbBox*> &viaTable, bool m1Vias);
	void writeViaName(char *nodeName, dbBox *v, uint level, const char *post);
	void writeViaName(FILE *fp, dbBox *v, uint level, const char *post);
	void writeViaNameCoords(FILE *fp, dbBox *v);
	// 041311D END
	float computeViaResistance(dbBox *viaBox, uint &cutCount);
	// 041511D END
	void printItermNodeSubCkt(FILE *fp, std::vector<uint> &iTable);
	void printViaNodeSubCkt(FILE *fp, std::vector<dbBox*> &viaTable);
	// 042711D END

	// 061711D BEGIN
	uint mergeStackedVias(FILE *fp, dbNet *net, std::vector<dbBox *> &viaTable, dbBox *botVia, FILE *fp1=NULL);
	// 061711D END
	// 061911D BEGIN
	uint  stackedViaConn(FILE *fp, std::vector<dbBox *> &allViaTable);
	bool skipSideMetal(std::vector<dbBox *> &viaTable, uint level, dbNet *net, adsRect *w);
	bool overlapWithMacro(adsRect &w);
	// 061911D END

	// 062511D BEGIN
	void powerWireConn(adsRect *w, uint dir, dbTechLayer *layer, dbNet *net);
	const char * getBlockType(dbMaster *m);
	void sortViasXY(uint dir, std::vector<dbBox *> &viaTable);
	// 062511D END
	// 063011D BEGIN
	void writeViaRes(FILE *fp, dbNet *net, uint level);
	void addUpperVia(uint ii, dbBox *v);
	void writeViaResistors(FILE* fp, uint ii, FILE *fp1, bool skipWireConn=NULL);
	// 063011D END
	// 071211D BEGIN
	void writeGeomHeader(FILE *fp, char *vdd);
	void writeResNode(char *nodeName, dbCapNode *capNode, uint level);
	float micronCoords(int xy);
	void writeSubcktNode(char *capNodeName, bool highMetal, bool vdd);
	void setPowerExtOptions(bool skip_power_stubs, const char *exclude_cells, bool skip_m1_caps, const char * f);
	bool markExcludedCells();
	float distributeCap(FILE *fp, dbNet *net);

	// 071211D END
	// 021712D BEGIN
	uint readPowerSupplyCoords(char * filename);
	uint addPowerSources(std::vector<dbBox *> &viaTable, bool power, uint level, adsRect *powerWire);
	char *getPowerSourceName(bool power, uint level, uint vid);
	char *getPowerSourceName(uint level, uint vid);
	void writeViaInfo(FILE *fp, bool power);
	void addPowerSourceName(uint ii, char *sname);
	// 021712D END
	// 062212D
	void writeResCoords(FILE *fp, dbNet *net, uint level, uint width, uint dir);
	void writeViaName_xy(char *nodeName, dbBox *v, uint bot, uint top, uint level,const char *post="");
	void writeInternalNode_xy(dbCapNode *capNode, FILE *fp);
	void writeInternalNode_xy(dbCapNode *capNode, char *buff);
	void createNode_xy(dbCapNode *capNode, int x, int y, int level, dbITerm *t=NULL);
	//void setNodeCoords_xy(FILE *fp, dbNet *net);
	uint setNodeCoords_xy(dbNet *net, int level);
	bool sameJunctionPoint(int xy[2], int BB[2], uint width, uint dir);

	// 071912D
	bool fisrt_markInst_UserFlag(dbInst *inst, dbNet *net);

	// 093012D
	bool matchLayerDir(dbBox *rail, dbTechLayerDir layerDir, int level, bool debug);
	// 100512D
	void addSubcktStatement(char * cirFile1, char *subcktFile1);
	void setPrefix(char *prefix);
	uint getITermPhysicalConn(uint dir, uint level, dbWireEncoder &encoder, dbWire * wire, dbNet *net, int *xy, int *xy2);
	void getSpecialItermShapes(dbInst *inst, dbNet *specialNet, uint dir, uint level, int *xy, int *xy2, std::vector<adsRect *> & rectTable, std::vector<dbITerm *> & itermTable);
	bool topHierBlock();

	// 101812D
	void writeNegativeCoords(char *buf, int netId, int x, int y, int level, const char *post="");

	// 101912D
	void writeViasAndClose(dbNet *net, bool m1Vias);
	void closeNanoFilesDomainVDD(char *netName);
	void closeNanoFilesDomainGND(char *netName);
	void netDirPrefix(char*, char*);
	FILE* openNanoFileNet(char*, const char*, const char*, const char*, const char*);
	void openNanoFilesDomain(ads::dbNet*);
	void addSubcktStatementDomain(char*, char*, char*);
	void initMappingTables();
	void allocMappingTables(int n1, int n2, int n3);
	// 102812D
	uint addSboxesOnSearch(dbNet *net);
	adsRect *getRect_SBox(Ath__array1D<uint> *table, uint ii, bool filter, uint dir, uint &maxWidth);
	uint mergePowerWires(uint dir, uint level, std::vector<adsRect *> &mergeTable);
	void railConnOpt(dbNet *net);
	uint initPowerSearch();
	uint overlapPowerWires(std::vector<adsRect *> &mergeTableHi, std::vector<adsRect *> &mergeTableLo, std::vector<adsRect *> &resultTable);
	dbBox *createMultiVia(uint top, uint bot, adsRect *r);
	void mergeViasOnMetal_1(adsRect *w, dbNet *pNet, uint level, std::vector<dbBox *> &viaTable);
	uint addGroupVias(uint level, adsRect *w, std::vector<dbBox *> &viaTable);
	uint mergeStackedViasOpt(FILE *fp, dbNet *net, std::vector<dbBox *> &viaSearchTable, dbBox *botVia, FILE *fp1, uint stackLevel=1);
	// 111112D
	dbCapNode * getITermPhysicalConnRC(dbCapNode *srcCapNode, uint level, uint dir, dbNet *net, int *xy, int *xy2, bool macro);
	uint viaAndInstConnRC(uint dir, uint width, dbTechLayer *layer, dbNet *net, dbNet *orig_power_net, adsRect *w, bool skipSideMetalFlag);
	void powerWireConnRC(adsRect *w, uint dir, dbTechLayer *layer, dbNet *net);
	dbCapNode * getITermConnRC(dbCapNode *srcCapNode, uint level, uint dir, dbNet *net, int *xy, int *xy2);
	int _last_node_xy[2];
	dbCapNode* getPowerCapNode(dbNet *net, int xy, uint level);
	dbCapNode *makePowerRes(dbCapNode *srcCap, uint dir, int xy[2], uint level, uint width, uint objId, int type);
	void createNode_xy_RC(char *buf, dbCapNode *capNode, int x, int y, int level);
	bool _wireInfra;
	void writeResNodeRC(char *nodeName, dbCapNode *capNode, uint level);
	void writeResNodeRC(FILE *fp, dbCapNode *capNode, uint level);
	double writeResRC(FILE *fp, dbNet *net, uint level, uint width, uint dir, bool skipFirst,bool reverse, bool onlyVias, bool caps, int xy[2]);
	void writeCapNodesRC(FILE *fp, dbNet *net, uint level, bool onlyVias, bool skipFirst);
	void writeViaResistorsRC(FILE *fp, uint ii, FILE *fp1);
	void viaTagByCapNode(dbBox *v, dbCapNode *cap);
	char * getViaResNode(dbBox *v, char *propName);
	void writeMacroItermConns(dbNet *net);
	void setupDirNaming();
	adsRect _extMaxRect;
	bool filterPowerGeoms(dbSBox * s, uint targetDir, uint &maxWidth);

	// 031313D
	uint iterm2Vias_cells(dbInst *inst, dbITerm *connectedPowerIterm);
	void writeCapNodesRC(FILE *fp, dbNet *net, uint level, bool onlyVias, std::vector<dbCapNode *> &capNodeTable);
	void writeOneCapNode(FILE *fp, dbCapNode *capNode, uint level, bool onlyVias);

	//032613D
	void findViaMainCoord(dbNet *net, char *buff);
	void replaceItermCoords(dbNet *net, uint dir, int xy[2]);

	//041713D
	void formOverlapVias(std::vector<adsRect *> mergeTable[16], dbNet *pNet);


};

END_NAMESPACE_ADS

#endif




