# Make Program
import re
from pyparsing import Word, alphas, nums, Literal, Keyword, Optional, OneOrMore, delimitedList, Group, ParseException, quotedString, Forward, StringStart, StringEnd

definitionFile = "ProteusDef.b"

structsSpec={}
MainPrgSpec={}
structNames=[]
structPtrs ={}
parserGlobalText=""

def initProgramSkeleton():
    structsSpec.clear()
    MainPrgSpec.clear()
    structNames=[]
    parserGlobalText=""

def addStruct(name):
    if(name in structsSpec):
        print "The struct '", name, "' is being added but already exists."
        exit(1)
    structsSpec[name]={'name':name, "attrList":[], "attr":{}, "fields":[]}
    print "ADDED STRUCT: ", name

def addField(structName, kindOfField, fieldType, fieldName):
    if (kindOfField=="var" or kindOfField=="ptr" or kindOfField=="dblList"):
        structsSpec[structName]["fields"].append({'kindOfField':kindOfField, 'fieldType':fieldType, 'fieldName':fieldName})
    else:
        print "When adding a Field to ", structName, ", invalid field type: ", kindOfField,"."
        exit(1)
    print "    ADDED FIELD:\t", kindOfField, fieldName

def addFlag(structName, flagName):
    structsSpec[structName]["fields"].append({'kindOfField':'flag', 'fieldName':flagName})
    print "    ADDED FLAG: \t", flagName


def addMode(structName, modeName, enumList):
    structsSpec[structName]["fields"].append({'kindOfField':'mode', 'fieldName':modeName, 'enumList':enumList})
    print "    ADDED MODE:\t", modeName


def addFunc(structName, funcName, returnType, funcText):
    structsSpec[structName]["fields"].append({'kindOfField':'func', 'funcText':funcText, 'returnType':returnType, 'fieldName':funcName})
    print "    ADDED FUNCTION:\t", funcName


def addMartialType():
    print "ADDED MARTIAL: "


def CreatePointerItems(structName):
    structPtrs[structName]=structName;
    print "ADDED AUTO-POINTER: "+structName


def CreateConstructorsDestructors():
    # Includes martialing constructors and copy operators + DeepCopy or ShallowCopy
    print "ADDED CONSTRUCTS: "


def CreateParserPrinterItems():
    print "ADDING PARSER + PRINTER... "


def CreatePercistanceItems():
    print "ADDING Percistance... "


def CreateDumpFunction(structName, fieldList):
    print "ADDING DUMP FUNCTION... "

def CreateEventHandler(structName, ruleList):
    print "ADDING EVENT HANDLER... "

def CreateMainFunction():
    print "ADDING MAIN(): "

def FillStructFromText(structName, structDefString):
    print "POPULATING STRUCT ", structName, " FROM TEXT..."

    #Split struct body
    structBodyText=structDefString
    items=None
    while(1):
        fieldTypeM=re.match("\s*(\w+)\s*:\s*", structBodyText)
        if fieldTypeM is None:
            break
        fieldType=fieldTypeM.group(1);
        #print fieldType
        if(fieldType=="flag"):
            items=re.match("\s*(\w+)\s*:\s*(\w+)", structBodyText)
            addFlag(structName, items.group(2))
        elif fieldType=="mode":
            items=re.match("\s*(\w+)\s*:\s*(\w+)\s*\[([\w\s,]*)\]", structBodyText)
            #print "<<<<<<<"+items.group(3)+">>>>>>>>>"
            fieldsName=items.group(2)
            enumList=re.findall("\w+", items.group(3))
            addMode(structName, fieldsName, enumList)
        elif (fieldType=="var"):
            items=re.match("\s*(\w+)\s*:\s*(\w+)\s*([\w\s<> \*,\[\]]*);", structBodyText)
            fieldsType=items.group(2)
            fieldsName=items.group(3)
            addField(structName, "var", fieldsType, fieldsName)
        elif fieldType=="ptr":
            items=re.match("\s*(\w+)\s*:\s*(\w+)\s*(\w*)", structBodyText)
            fieldsType=items.group(2)
            fieldsName=items.group(3)
            addField(structName, "ptr", fieldsType, fieldsName)
        elif fieldType=="dblList":
            items=re.match("\s*(\w+)\s*:\s*(\w+)\s*(\w*)", structBodyText)
            fieldsType=items.group(2)
            fieldsName=items.group(3)
            addField(structName, "dblList", fieldsType, fieldsName)
        elif fieldType=="func":
            items=re.match("\s*(\w+)\s*:\s*(.+?)\s*:\s*(.+?)\s*END", structBodyText, re.DOTALL)
            returnType=items.group(2)
            funcsText=items.group(3)
            fieldsName="FUNC"
            addFunc(structName, fieldsName, returnType, funcsText)

        structBodyText=structBodyText[items.end():]

def getNameSegInfo(structName, fieldName):
    structToSearch = structsSpec[structName]
    if not structToSearch: print "struct ", structName, " not found."; exit(2);
    fieldListToSearch = structToSearch['fields']
    if not fieldListToSearch: print "struct's fields ", structName, " not found."; exit(2);
    for fieldRec in fieldListToSearch:
        if fieldRec['fieldName']==fieldName:
            print "FOR ", structName, ',', fieldName, ", returning", fieldRec
            return fieldRec
    return None

def getFieldInfo(structName, fieldNameSegments):
    # return [kind-of-last-element,  reference-string, type-of-last-element]
    structKind=""
    prevKind="ptr"
    structType=""
    referenceStr=""
    print "    Getting Field Info for:", structName, fieldNameSegments
    for fieldName in fieldNameSegments:
        REF=getNameSegInfo(structName, fieldName)
        if(REF):
            print "    REF:", REF
            if 'kindOfField' in REF:
                structKind=REF['kindOfField']
                if(prevKind=="ptr"): joiner='->'
                else: joiner='.'
                if (structKind=='flag'):
                    referenceStr+=joiner+"flags"
                elif structKind=='mode':
                    referenceStr+=joiner+'flags'
                elif structKind=='var':
                    referenceStr+= joiner+fieldName
                    structType=REF['fieldType']
                elif structKind=='ptr':
                    referenceStr+= joiner+fieldName
                    structType=REF['fieldType']
                prevKind=structKind
            structName=structType
        else: print "Problem getting name seg info:", structName, fieldName; exit(1);
    return [structKind, referenceStr, structType]

def getValueString(structName, valItem):
    if isinstance(valItem, list):
        return getFieldInfo(structName, valItem)
    else:
        return (["", valItem])

def getActionTestStrings(structName, action):
    print "################################ Getting Action and Test string for", structName, "ACTION:", action
    LHS=getFieldInfo(structName, action[0])
    print "LHS:", LHS
    RHS=getValueString(structName, action[2])
    leftKind=LHS[0]
    actionStr=""; testStr="";
    if leftKind =='flag':
        print "ACTION[0]=", action[2][0]
        actionStr="SetBits(ITEM->flags, "+action[0][-1]+", "+ action[2][0]+");"
        testStr="(flags&"+action[0][0]+")"
    elif leftKind == "mode":
        ITEM="ITEM"
        actionStr="SetBits("+ITEM+LHS[1]+", "+action[0][-1]+"Mask, "+ action[2][0]+");"
        testStr="((flags&"+action[0][-1]+"Mask)"+">>"+action[0][-1]+"Offset) == "+action[2][0]
    elif leftKind == "var":
        actionStr="ITEM"+LHS[1]+action[1]+action[2][0]+'; '
        testStr=action[0][0]+"=="+action[2][0]
    elif leftKind == "ptr":
        print "PTR - ERROR: CODE THIS"
        exit(2)
    return ([leftKind, actionStr, testStr])


def GenerateProgram():
    print "Generating Program... "

#/////////////////////////////////////////////////  R o u t i n e s   t o   G e n e r a t e   P a r s e r s
parserString = ""

def parseParserSpec():
    ParseElement = Forward()
    fieldsName=Word(alphas+'_0123456789')
    FieldSpec = Group(fieldsName + Optional((Literal('.') | Literal('->')).suppress() + fieldsName))
    ValueSpec = FieldSpec | Word(nums) | (Keyword('true') | Keyword('false')) | quotedString
    WhitespaceEL = Keyword("WS")
    SetFieldStmt = Group(FieldSpec + '=' + ValueSpec)
    PeekNotEL = "!" + quotedString
    LiteralEL = quotedString
    StructEL  = '#'+Word(alphas)
    ListEL = Group((Literal("+") | Literal("*")) + ParseElement)
    OptionEL = Group("<" + ParseElement + ">")
    CoFactualEL  = "(" + Group(ParseElement + "<=>" + Group(OneOrMore(SetFieldStmt + Literal(';').suppress())))  + ")"
    SequenceEL   = "{" + Word(alphas) + Group(OneOrMore(ParseElement)) + "}"
    AlternateEl  = "[" + Word(alphas) + Group(OneOrMore(ParseElement + Optional("|").suppress())) + "]"
    ParseElement <<= (Group(SequenceEL) | Group(AlternateEl) | Group(CoFactualEL) | ListEL | OptionEL | Group(StructEL) | LiteralEL | Group(PeekNotEL) | WhitespaceEL)
    structParserSpec = Keyword("StructParser") + Word(alphas) + "=" + ParseElement
    structParserSpec=structParserSpec.setDebug()
    StartSym = StringStart() + Literal("{").suppress() + OneOrMore(Group(structParserSpec)) +Literal("}").suppress()
    return StartSym

tagModifier=1
def TraverseParseElement(structName, parseEL, BatchParser, PulseParser, PrintFunc, indent):
    # BatchParser[0] = text of parsing function being built.
    # BatchParser[1] = text of functions to add to the class.
    global tagModifier
    indent2=indent+"    "
    batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
    if(type(parseEL)==type("")):
        if(parseEL[0]=='"'):
            batchArgs[1] += "nxtTok(cursor, "+parseEL+")"
            printerArgs[1] ='S+='+parseEL+';'
        elif(parseEL=='WS'):
            batchArgs[1] += "RmvWSC(cursor)"

        else:
            batchArgs[1] +=  "<" +parseEL+ ">"
    elif(parseEL[0]=='('):
        #print indent, "Co-Factual"
        actionStr=""; testStr="";
        print "PREPING ACTION..."
        count=0
        for action in parseEL[1][2]:
            print action
            resultStrs=getActionTestStrings(structName, action)
            actionStr+=resultStrs[1]
            if(count>0): testStr+=" && "
            count+=1
            testStr+=resultStrs[2]

        Item = parseEL[1][0]
        print "=========================>",Item
        batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
        TraverseParseElement(structName, Item, batchArgs, pulseArgs, printerArgs, indent2)
        batch0="    func: bool: parseCoFactual_"+str(tagModifier)+"(streamSpan* cursor, "+structName+"Ptr ITEM){\n"
        batch0+= indent+"if(" + batchArgs[1] + ") {"+actionStr+"} else {MARK_ERROR; return false;}\n"
        batchArgs[0]+="\n"+batch0+"    }; END\n"
        batchArgs[1]="parseCoFactual_"+str(tagModifier)+"(cursor, ITEM)"

        printerArgs[1]=indent+"if("+testStr+"){"+printerArgs[1]+"}\n"
        tagModifier+=1

    elif(parseEL[0]=='{'):
        #print indent, "Sequence"
        tagModifierS=parseEL[1]
        print1=""
        batch1=""
        batch0="";
        batch0+="    func: bool: parseSequence_"+tagModifierS+"(streamSpan* cursor, "+structName+"Ptr ITEM){\n"
        for firstItem in parseEL[2]:
            batch0 += indent+"if(!"
            batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
            TraverseParseElement(structName, firstItem, batchArgs, pulseArgs, printerArgs, indent2)
            batch1+=batchArgs[0]
            batch0+=batchArgs[1]
            batch0+=") {MARK_ERROR; return false;}\n"

            print1+=printerArgs[1]

        batch0+=indent+"return true;\n    }; END\n\n\n"
        batchArgs[0]=batch1+"\n"+batch0
        batchArgs[1]="parseSequence_"+tagModifierS+"(cursor, ITEM)"
        printerArgs[1]=print1

    elif(parseEL[0]=='['):
        #print indent, "OneOf"
        tagModifierS=parseEL[1]
        batch1=""
        batch0="    func: bool: parseAltList_"+tagModifierS+"(streamSpan* cursor, "+structName+"Ptr ITEM){\n"
        for firstItem in parseEL[2]:
            batch0 += indent+"if("
            batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
            TraverseParseElement(structName, firstItem, batchArgs, pulseArgs, printerArgs, indent2)
            batch1+=batchArgs[0]
            batch0+=batchArgs[1]
            batch0+=") {return true;}\n"
        batch0+=indent+"return false;\n    }; END\n\n\n"
        batchArgs[0]=batch1+"\n"+batch0
        batchArgs[1]="parseAltList_"+tagModifierS+"(cursor, ITEM)"
    elif(parseEL[0]=='<'):   # OPTIONAL
        batch1=""
        batch0="";
        batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
        TraverseParseElement(structName, parseEL[1], batchArgs, pulseArgs, printerArgs, indent2)
        batch1+=batchArgs[0]
        batch0+=batchArgs[1]
        batchArgs[1] = "(" + batch0 + "||true)"

    elif(parseEL[0]=='!'):   # NOT LITERAL
        batch1=""
        batch0="";
        batchArgs=["", ""]; pulseArgs=["", ""]; printerArgs=["", ""];
        TraverseParseElement(structName, parseEL[1], batchArgs, pulseArgs, printerArgs, indent2)
        batch1+=batchArgs[0]
        batch0+=batchArgs[1]
        batchArgs[1] = "!(" + batch0 + ")"
    elif(parseEL[0]=='#'):   # Sub-STRUCT
        FieldData=getFieldInfo(structName, [parseEL[1]])
        sType=FieldData[2]
        printCmd=""
        if(sType==""): sType=structName
        if(FieldData[0]=="ptr"):
            sField="ITEM->"+parseEL[1]
            printCmd="->printToString();\n"
        else:
            sField = sType+"Ptr(&(ITEM->"+parseEL[1]+"))"
            printCmd=".printToString();\n"
        batchArgs[1] = "parse_"+sType+"(cursor, "+sField+")"
        printerArgs[1] = indent + "S += "+parseEL[1]+printCmd
    else:
        print indent, parseEL

    BatchParser[0]+=batchArgs[0]; BatchParser[1]+=batchArgs[1];
    PulseParser[0]+=pulseArgs[0]; PulseParser[1]+=pulseArgs[1];
    PrintFunc[0]+=printerArgs[0]; PrintFunc[1]+=printerArgs[1];

def generateParser(parserSpec, startSymbol):
    AST = parseParserSpec()
    try:
        results = AST.parseString(parserSpec, parseAll=True)
        print parserSpec, " ==> ", results
    except ParseException, pe:
        print "ERROR Creating Grammar:", parserSpec, " ==> ", pe
        exit()
    else:
        print "SUCCESS Creating Grammar.\n"
        BatchParserUtils="" #// Batch Parsing utility parsing functions:
        PulseParserUtils=""
        BatchParser = "    func: bool: BatchParse(streamSpan* cursor, "+startSymbol + "Ptr ITEM){\n    parse_"+startSymbol+"(cursor, ITEM);\n}; END\n"
        PulseParser = "    func: bool: PulseParse"+startSymbol+"(){\n"+"    }; END\n"

        for STRCT in results:
            print "struct ",STRCT[1],"="
            batchArgs=["",""]; PulseArgs=["",""]; printArgs=["",""];
            TraverseParseElement(STRCT[1], STRCT[3], batchArgs, PulseArgs, printArgs, "        ")
            CreatePointerItems(STRCT[1])

            BatchParserUtils+=batchArgs[0]
            BatchParser+="    func: bool: parse_"+STRCT[1]+"(streamSpan* cursor, "+STRCT[1]+"Ptr ITEM){\n        "+batchArgs[1]+";\n    }; END\n"

            PrinterFunc = '    func: string: printToString(){\n        string S="";\n' + printArgs[1] + "        return S;\n    }; END\n"
            FillStructFromText(STRCT[1], PrinterFunc)


        BatchParserFuncs= BatchParserUtils + "\n\n" + BatchParser
        PulseParserFuncs= PulseParserUtils + "\n\n" + PulseParser


    global parserGlobalText; parserGlobalText = r"""

const int bufmax = 32*1024;
#define streamEnd (stream->eof() || stream->fail())
#define ChkNEOF {if(streamEnd) {flags|=fileError; statusMesg="Unexpected End of file"; break;}}
#define getbuf(cursor, c) {ChkNEOF; for(p=0;(c) && !streamEnd ;buf[p++]=streamGet(cursor)){if (p>=bufmax) {flags|=fileError; statusMesg="String Overflow"; break;}} buf[p]=0;}
#define nxtTok(cursor, tok) nxtTokN(cursor, 1,tok)

#define U8_IS_SINGLE(c) (((c)&0x80)==0)

UErrorCode err=U_ZERO_ERROR;
const UnicodeSet TagChars(UnicodeString("[[:XID_Continue:]']"), err);
const UnicodeSet TagStarts(UnicodeString("[:XID_Start:]"), err);
bool iscsymOrUni (char nTok) {return (iscsym(nTok) || (nTok&0x80));}
bool isTagStart(char nTok) {return (iscsymOrUni(nTok)&&!isdigit(nTok)&&(nTok!='_'));}
bool isAscStart(char nTok) {return (iscsym(nTok)&&!isdigit(nTok));}
bool isBinDigit(char ch) {return (ch=='0' || ch=='1');}
const icu::Normalizer2 *tagNormer=Normalizer2::getNFKCCasefoldInstance(err);
#define MARK_ERROR "ERROR"

bool tagIsBad(string tag, const char* locale) {
    UErrorCode err=U_ZERO_ERROR;
    if(!TagStarts.contains(tag.c_str()[0])) return 1; // First tag character is invalid
    if((size_t)TagChars.spanUTF8(tag.c_str(), -1, USET_SPAN_SIMPLE) != tag.length()) return 1;
    USpoofChecker *sc = uspoof_open(&err);
    uspoof_setChecks(sc, USPOOF_SINGLE_SCRIPT|USPOOF_INVISIBLE, &err);
    uspoof_setAllowedLocales(sc, locale, &err);
    int result=uspoof_checkUTF8(sc, tag.c_str(), -1, 0, &err);
    return (result!=0);
}
"""
    parserFields=r"""
        flag: fullyLoaded
        flag: userMode
        flag: parseError
        flag: fileError
        flag: isParsing
        var:  string statusMesg;

        var: char nTok;
        var: char buf[bufmax];

        var: istream *stream;
        var: string streamName;
        var: string streamPath;
        var: uint line;
        var: char prevChar;

        var: string textStreamed;
        var: posRecStore textPositions;
        var: vector<int64_t> *punchInOut;
        ptr: attrStore attributes

        var: infonPtr ti;

        func: none: ~infonParser(){delete punchInOut;};   END

        func: char: streamGet(streamSpan* cursor){
            char ch;
            switch(flags&(fullyLoaded|userMode)){
                case(userMode):  // !fully loaded | userMode
                    while(cursor->offset > textStreamed.size()){
                        ch=stream->get();
                        textStreamed += ch;
                        if(stream->eof()){
                            flags|=fullyLoaded;
                        } else if(stream->fail()){
                            flags|=fileError;
                            return 0;
                        }
                    }
                    break;
                case(0): // !fullyLoaded | !userMode
                    stream->seekg(0, std::ios::end);
                    int streamSize=stream->tellg();
                    if(streamSize<=0) {printf("Problem with stream Stream size is %i.\n", streamSize); exit(1);}
                    textStreamed.resize(streamSize);
                    stream->seekg(0, std::ios::beg);
                    stream->read((char*)textStreamed.data(), textStreamed.size());
                    flags|=fullyLoaded;
                    break;
            }
            return textStreamed[cursor->offset];
        };  END

        func: void: scanPast(streamSpan* cursor, char* str){
            char p; char* ch=str;
            while(*ch!='\0'){
                p=streamGet(cursor);
                if (streamEnd){
                    if (strcmp(str,"<%")==0) return;
                    throw (string("Expected String not found before end-of-file: '")+string(str)+"'").c_str();
                }
                if (*ch==p) ch++; else ch=str;
                if (p=='\n') ++line;
            }
        };               END

        func: bool: chkStr(streamSpan* cursor, const char* tok){
            int startPos=cursor->offset;
            if (tok==0) return 0;
            for(const char* p=tok; *p; p++) {
                if ((*p)==' ') RmvWSC(cursor);
                else if ((*p) != streamGet(cursor)){
                    cursor->offset=startPos;
                    return false;
                }
            }
            return true;
        };           END

        func: void: streamPut(int nChars){for(int n=nChars; n>0; --n){stream->putback(textStreamed[textStreamed.size()-1]); textStreamed.resize(textStreamed.size()-1);}}; END

        func: const char* :nxtTokN(streamSpan* cursor, int n, ...){
            char* tok; va_list ap; va_start(ap,n); int i,p;
            for(i=n; i; --i){
                tok=va_arg(ap, char*); nTok=WSPeek(cursor);
                if(strcmp(tok,"cTok")==0) {if(iscsym(nTok)&&!isdigit(nTok)&&(nTok!='_')) {getbuf(cursor, iscsym(pPeek(cursor))); break;} else tok=0;}
                else if(strcmp(tok,"unicodeTok")==0) {if(isTagStart(nTok)) {getbuf(cursor, iscsymOrUni(pPeek(cursor))); break;} else tok=0;}
                else if(strcmp(tok,"<abc>")==0) {if(chkStr(cursor, "<")) {getbuf(cursor, (pPeek(cursor)!='>')); chkStr(cursor, ">"); break;} else tok=0;}
                else if(strcmp(tok,"123")==0) {if(isdigit(nTok)) {getbuf(cursor, (isdigit(pPeek(cursor))||pPeek(cursor)=='.')); break;} else tok=0;}
                else if(strcmp(tok,"0x#")==0) {if(isxdigit(nTok)) {getbuf(cursor, (isxdigit(pPeek(cursor))||pPeek(cursor)=='.')); break;} else tok=0;}
                else if(strcmp(tok,"0b#")==0) {if(isBinDigit(nTok)) {getbuf(cursor, (isBinDigit(pPeek(cursor))||pPeek(cursor)=='.')); break;} else tok=0;}
                else if (chkStr(cursor, tok)) break; else tok=0;
            }
            va_end(ap);
            return tok;
        };        END


        func: char: pPeek(streamSpan* cursor){
            while(textStreamed.length() < cursor->offset){
                if(flags&fullyLoaded){return 0;}
                char ch=stream->get();
                if(stream->eof()){}
                if(stream->fail()){}
                textStreamed + ch;
            }
            return(textStreamed[cursor->offset]);
        } END

        func: char: WSPeek(streamSpan* cursor){RmvWSC(cursor); return pPeek(cursor);} END

        func: bool: RmvWSC(streamSpan* cursor){ //, attrStorePtr attrs=0){
            char p,p2;
            for (p=pPeek(cursor); (p==' '||p=='/'||p=='\n'||p=='\r'||p=='\t'||p=='%'); p=pPeek(cursor)){
                if (p=='/') {
                    streamGet(cursor); p2=pPeek(cursor);
                    if (p2=='/') {
        //              punchInOut->push_back(textStreamed.size()-1);  // Record start of line comment.
                        string comment="";
                        for (p=pPeek(cursor); !streamEnd && p!='\n'; p=pPeek(cursor)) comment+=streamGet(cursor);
        //                punchInOut->push_back(-textStreamed.size());  // Record end of line comment.
                    /*    if (attrs){
                            if     (comment.substr(1,7)=="author=") attrs->a.insert(pair<string,string>("author",comment.substr(8)));
                            else if(comment.substr(1,6)=="image=") attrs->a.insert(pair<string,string>("image",comment.substr(7)));
                            else if(comment.substr(1,9)=="engTitle=") attrs->a.insert(pair<string,string>("engTitle",comment.substr(10)));
                            else if(comment.substr(1,7)=="import=") attrs->a.insert(pair<string,string>("import",comment.substr(8)));
                            else if(comment.substr(1,8)=="summary=") attrs->a.insert(pair<string,string>("summary",comment.substr(9)));
                            else if(comment.substr(1,5)=="link=") attrs->a.insert(pair<string,string>("link",comment.substr(6)));
                            else if(comment.substr(1,9)=="category=") attrs->a.insert(pair<string,string>("category",comment.substr(10)));
                            else if(comment.substr(1,7)=="posted=") attrs->a.insert(pair<string,string>("posted",comment.substr(8)));
                            else if(comment.substr(1,8)=="updated=") attrs->a.insert(pair<string,string>("updated",comment.substr(9)));
                        } */
                    } else if (p2=='*') {
                        punchInOut->push_back(textStreamed.size()-1);  // Record start of block comment.
                        for (p=streamGet(cursor); !streamEnd && !(p=='*' && pPeek(cursor)=='/'); p=streamGet(cursor))
                            if (p=='\n') ++line;
                        if (streamEnd) throw "'/*' Block comment never terminated";
                        streamGet(cursor);
                        punchInOut->push_back(-(textStreamed.size()));  // Record end of block comment.
                    } else {streamPut(1); return true;}
                } else if (p=='%'){
                    streamGet(cursor); p2=pPeek(cursor);
                    if(p2=='>'){scanPast(cursor, (char*)"<%");} else {streamPut(1); return true;}
                }
                if (streamGet(cursor)=='\n') {++line; prevChar='\n';} else prevChar='\0';
            }
            return true;
        };      END


        } END



        func: bool: doRule(infon* i){
            uint64_t parsePhase = flags&InfParsePhaseMask;
            if(parsePhase==iStartParse){
                if(nxtTok(i->curPos, "@")){i->flags |= asDesc;}
                if(nxtTok(i->curPos, "`")){i->flags |= toExec;}
            } else if(parsePhase==iParseIdents){
            } else if(parsePhase==iParseFuncArgs){
            }
            return 1;
        } END

    """

    ParserStructsName = startSymbol+"Parser"
    addStruct(ParserStructsName)
    FillStructFromText(ParserStructsName, BatchParserFuncs)
    FillStructFromText(ParserStructsName, parserFields)
    structNames.append(ParserStructsName)



#/////////////////////////////////////////////////  R o u t i n e s   t o   G e n e r a t e  a n   E v e n t H a n d l e r
def generateEventHandler():
    EventHandlerCode = r"""
        mode: coreRunState [corePreInit, coreParsing, coreRunning]

        var:  infon topInfon;
        var:  infonParser parser;
        var:  bool doneYet;

        func: bool: doRule(infon* i){
            switch(i->flags&coreRunStateMask){
                case corePreInit:    break;
                //case coreParsing: parser.doRule(i);    break;
                case coreRunning: topInfon.doRule(i);  break;
                default:exit(2);
            }
        } END

        func: bool: pollEvent(infon** inf){return 0;}END
        func: int: eventLoop(){
            uint64_t now=0, then=0; //SDL_GetTicks();
            uint64_t frames=0;
            infon* inf;
            while (!doneYet){
                 ++frames;
                while (pollEvent(&inf)) {
                    doRule(inf);
                }
                if(doneYet) continue;
  /*              if(portal->needsToBeDrawn) portal->needsToBeDrawn=false; else continue;
                DrawScreen(portal);
                SDL_RenderPresent(portal->viewPorts->renderer); //OgrRenderWnd->swapBuffers(false);
                update(portals[0]);  */
            }
       //     if ((now = SDL_GetTicks()) > then) printf("\nFrames per second: %2.2f\n", (((double) frames * 1000) / (now - then)));

        } END
    """

    addStruct("eventHandler")
    FillStructFromText("eventHandler", EventHandlerCode)
    structNames.append("eventHandler")
#/////////////////////////////////////////////////  R o u t i n e s   t o   G e n e r a t e  " m a i n ( ) "
mainFuncCode=r"// No Main given"
def generateMainFunction():
    global dataTags
    dataTags['Include'] += ",<signal.h>"
    global mainFuncCode
    mainFuncCode=r"""
static void reportFault(int Signal){cout<<"\nSegmentation Fault.\n"; fflush(stdout); abort();}

int main(int argc, char **argv){
    if(sizeof(int)!=4) {cout<<"WARNING! int size is "<<sizeof(int)<<" bytes.\n\n";}
    signal(SIGSEGV, reportFault);
fstream fileIn("testInfon.pr");
infonParser parser;
parser.stream=&fileIn;
streamSpan cursor;
infon topInfon;
infonPtr topInfonPtr(&topInfon);
parser.BatchParse(&cursor, topInfonPtr);
exit(0);
    eventHandler EvH;
    int ret=EvH.eventLoop();
//    EvH.shutDown();

    return ret;
}
"""

#############################################    L o a d / P a r s e   P r o g r a m   S p e c

def bitsNeeded(n):
    if n <= 1:
        return 0
    else:
        return 1 + bitsNeeded((n + 1) / 2)

def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

f=open(definitionFile)
d = f.read()
f.close()

#### Remove comments from the string
d=comment_remover(d)

dataTags={}

############## Split string into var and struct sections

varsStructs=re.compile(r"(.*?)(struct.+)", re.DOTALL)
m1=varsStructs.match(d)
if m1 is None:
    print"NO MATCH\n"
    exit()

############## Set the variables into dataTags['varName']
varRe=re.compile(r"\s*(\w+)\s*:\s*`(.+?)`", re.DOTALL);
m=varRe.findall(m1.group(1))
for keyVal in m:
    dataTags[keyVal[0]]=keyVal[1]

for key in dataTags:
    print "=> "+key+": \t"+dataTags[key]

##############  Split structs section into struct defs and store in StructStrs
structsRe=re.compile(r"\s*struct ", re.DOTALL);
StructStrs=structsRe.split(m1.group(2))


##############   Load all the structs into structs[structname] = {name, attrList, structBody, attr:{}, fields:[]
SM=re.compile("\s*(\w+)\s*([\w, \n]*)\s*{(.+)}", re.DOTALL)
count=0
for StrS in StructStrs:
    count+=1
    if(count==1):
        continue
    #print StrS
    structFields=SM.match(StrS)
    if structFields is None:
        print "No MATCH for:"
        print "<"+StrS+">\n\n"
        exit()
    else:
        structName=structFields.group(1)
        structNames.append(structName)
        structBodyText=structFields.group(3)
        addStruct(structName)
        FillStructFromText(structName, structBodyText)  #  Extract each struct's attribute tags and fields.

#############################################   Add structs, fields, etc for each modifier
# modifiers can add structs, fields, etc.
modifierCmds = re.split("\|", dataTags['modifierCmds'])
for modifierCmd in modifierCmds:
    strippedCmd = modifierCmd.strip()
    print "MODIFIER: ",strippedCmd
    exec strippedCmd


#############################################   C r e a t e   A u x i l a r y   C o d e: enums, #defines, etc

structForwardDecls="\n";
structEnums="\n///////////////////////////////////////////////////\n////   E N U M E R A T I O N S \n\n"
structCode =""      # for storing the struct section of generated code.
structsData= {}
structsList= []


for struct in structNames:
    structsData[struct]={"needsFlags":False, "fields":[]}
    accessDefines=""
    structDef="    uint64_t flags;\n"
    print "Writing struct: ", struct
    bitCursor=0;
    needsFlagsVar=False
    structForwardDecls=structForwardDecls+"struct "+ struct +";   \t// Forward decl\n";
    structEnums = structEnums + "\n//// Enums for "+struct+":\n";
    for field in structsSpec[struct]['fields']:
        #print(field)
        kindOfField=field['kindOfField'];
        fieldName=field['fieldName'];
        if kindOfField=='flag':
            structsData[struct]["needsFlags"]=True
            structEnums += "const int "+fieldName +" = " + hex(1<<bitCursor) +";\n"
            bitCursor=bitCursor+1;
        elif kindOfField=='mode':
            structsData[struct]["needsFlags"]=True
            # calculate field and bit position
            enumSize= len(field['enumList'])
            numEnumBits=bitsNeeded(enumSize)
            #field[3]=enumSize;
            #field[4]=numEnumBits;
            enumMask=((1 << numEnumBits) - 1) << bitCursor

            structEnums = structEnums+"\nconst int "+fieldName +"Offset = " + hex(bitCursor) +";"
            structEnums = structEnums+"\nconst int "+fieldName +"Mask = " + hex(enumMask) +";"

            # enum
            count=0
            structEnums = structEnums+"\nenum " + fieldName +" {"
            for enumName in field['enumList']:
                structEnums = structEnums+enumName+"="+hex(count<<bitCursor)
                count=count+1
                if(count<enumSize): structEnums = structEnums+", "
            structEnums = structEnums+"};\n";

            structEnums =structEnums+'string ' + fieldName+'Strings[] = {"'+('", "'.join(field['enumList']))+'"};\n'
            # read/write macros
            accessDefines = accessDefines + "#define "+fieldName+"is(VAL) ((inf)->flags & )"
            # str array and printer


            bitCursor=bitCursor+numEnumBits;
        elif kindOfField=='var':
#           addField(struct, 'var', field['fieldType'], fieldName)
            structsData[struct]["fields"].append([field['fieldType'], fieldName])
        elif kindOfField=='ptr':
            structsData[struct]["fields"].append([field['fieldType']+"Ptr", fieldName])
            CreatePointerItems(field['fieldType'])
        elif kindOfField=='func':
            structsData[struct]["fields"].append(["FUNC", fieldName, field['returnType'], field['funcText']])
        else: print "\nWARNING!!! Invalid field type!\n";

    structsList.append(struct)

#############################################    S y n t h e s i s   P a s s
funcDefCode="// FUNCTION DEFINITIONS\n\n"

# Write structs to strings in C++
for structName in structsList:
    constructorInit=":"
    constructorArgs="    "+structName+"("
    if(structsData[structName]["needsFlags"]): structsData[structName]["fields"].append(["uint64_t", "flags", 1])
    if(structName in structPtrs):
        structsData[structName]["fields"].append(["uint", "refCnt", 0])
    structCode = structCode +"\nstruct "+structName+"{\n"
    count=0;
    for structField in structsData[structName]["fields"]:
        fieldType=structField[0]
        fieldName=structField[1]
        if fieldName=="FUNC":
            returnType=structField[2]
            if returnType=='none': returnType=""
            funcText = structField[3]
            parser=re.match("\s*(.+?\))", funcText)
            structCode += "    "+returnType + " " + parser.group(1)+";\n\n"
            funcDefCode += returnType + " " + structName +'::'+funcText+"\n\n"
        else:
            structCode += "    "+fieldType+" "+fieldName+";\n"
            if(fieldType[0:3]=="int" or fieldType[0:4]=="uint" or fieldType[-3:]=="Ptr"):
                constructorArgs += fieldType+" _"+fieldName+"=0,"
                constructorInit += fieldName+"("+" _"+fieldName+"),"
                count=count+1
            elif(fieldType=="string"):
                constructorArgs += fieldType+" _"+fieldName+'="",'
                constructorInit += fieldName+"("+" _"+fieldName+"),"
                count=count+1
    if(count>0):
        constructorInit=constructorInit[0:-1]
        constructorArgs=constructorArgs[0:-1]
    structCode += constructorArgs+")"+constructorInit+"{};\n" +"};\n"

# Write pointer code
structPtrCodeTop="//// Smart Pointer definitions:\n";
structPtrCodeEnd="";
for ptrStruct in structPtrs:
    structPtrCodeTop += r"typedef boost::intrusive_ptr<"+structPtrs[ptrStruct]+"> "+structPtrs[ptrStruct]+"Ptr;\n"
    structPtrCodeEnd += "\nvoid intrusive_ptr_add_ref("+structPtrs[ptrStruct]+"* p){++p->refCnt;} \nvoid intrusive_ptr_release("+structPtrs[ptrStruct]+"* p){if(--p->refCnt == 0) delete p;}\n"

################ Create Headers section
hdrString="";
includes = re.split("[,\s]+", dataTags['Include'])
for hdr in includes:
    hdrString+="\n#include "+hdr
hdrString += "\n\nusing namespace std; \n\n"

#############################################    G e n e r a t e   C o d e
headerFile  = "// "+dataTags['Title']+hdrString
headerFile += "string enumText(string* array, int enumVal, int enumOffset){return array[enumVal >> enumOffset];}\n";
headerFile += "#define SetBits(item, mask, val) {(item) &= ~(mask); (item)|=(val);}\n"
headerFile += structForwardDecls;
headerFile += dataTags['global'] + parserGlobalText

headerFile += "\nstruct infSource{\n    uint32_t offset, length;\n};\n"
headerFile += structEnums+"\n"+structPtrCodeTop+"\n" + structCode +"\n" + structPtrCodeEnd +"\n\n"
headerFile += funcDefCode
headerFile += parserString
headerFile += mainFuncCode
headerFile += '\n'

#print (headerFile)

fo=open(dataTags['FileName'], 'w')
fo.write(headerFile)
fo.close()
