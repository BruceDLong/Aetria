# Make Program
import re
from pyparsing import Word, alphas, nums, Literal, Keyword, Optional, OneOrMore, delimitedList, Group, ParseException, quotedString, Forward, StringStart, StringEnd

definitionFile = "ProteusDef.b"

structsSpec={}
MainPrgSpec={}
structNames=[]
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


def addFunc(structName, funcName, funcText):
	structsSpec[structName]["fields"].append({'kindOfField':'func', 'funcText':funcText, 'fieldName':funcName})
	print "    ADDED FUNCTION:\t", funcName


def addMartialType():
	print "ADDED MARTIAL: "


def CreatePointerItems():
	print "ADDED AUTO-POINTER: "


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
			items=re.match("\s*(\w+)\s*:\s*(.+?)\s*END", structBodyText, re.DOTALL)
			funcsText=items.group(2)
			fieldsName="FUNC"
			addFunc(structName, fieldsName, funcsText)

		structBodyText=structBodyText[items.end():]


def GenerateProgram():
	print "Generating Program... "

#/////////////////////////////////////////////////  R o u t i n e s   t o   G e n e r a t e   P a r s e r s
parserString = ""

def parseParserSpec():
    ParseElement = Forward()
    fieldsName=Word(alphas+'_')
    FieldSpec = fieldsName + Optional((Literal('.') | Literal('->')) + fieldsName)
    ValueSpec = FieldSpec | Word(nums) | (Keyword('true') | Keyword('false'))
    WhitespaceEL = Keyword("WS")
    SetFieldStmt = FieldSpec + '=' + ValueSpec
    PeekNotEL = "!" + quotedString
    LiteralEL = quotedString
    StructEL  = '#'+Word(alphas)
    DblListEL = FieldSpec
    CoFactualEL  = "(" + Group(OneOrMore((ParseElement | SetFieldStmt) + ';') + Optional(";")) + ")"
    SequenceEL   = "{" + Group(OneOrMore(ParseElement)) + "}"
    AlternateEl  = "[" + Group(OneOrMore(ParseElement + Optional("|"))) + "]"
    ParseElement <<= (Group(SequenceEL) | Group(AlternateEl) | Group(CoFactualEL) | Group(StructEL) | LiteralEL | Group(PeekNotEL) | WhitespaceEL)
    structParserSpec = Keyword("StructParser") + Word(alphas) + "=" + ParseElement
    #structParserSpec=structParserSpec.setDebug()
    StartSym = StringStart() + Literal("{").suppress() + OneOrMore(Group(structParserSpec)) +Literal("}").suppress()
    return StartSym

def TraverseParseElement(parseEL, indent):
	if(type(parseEL)==type("")):
		if(parseEL[0]=='"'):
			print indent, "check(", parseEL, ")"
		else:
			print indent, parseEL
	elif(parseEL[0]=='('):
		print indent, "CoFactuals"
		for firstItem in parseEL[1]:
			TraverseParseElement(firstItem, indent+"    ")

	elif(parseEL[0]=='{'):
		print indent, "Sequence"
		#bool done=false; while(!done){ done=true; // Do once but allow "break"
		for firstItem in parseEL[1]:
			TraverseParseElement(firstItem, indent+"    ")
			# bool parsedOK=<call to child test here>
			# if(!parserOK) {MARK ERROR; break;}
		#}
	elif(parseEL[0]=='['):
		print indent, "OneOf"
		for firstItem in parseEL[1]:
			TraverseParseElement(firstItem, indent+"    ")
	elif(parseEL[0]=='!'):
		print indent, '!'+parseEL[1]
	elif(parseEL[0]=='#'):
		print indent, '#'+parseEL[1]
	else:
		print indent, parseEL

def generateParser(parserSpec, startSymbol):
    AST = parseParserSpec()
    try:
        results = AST.parseString(parserSpec, parseAll=True)
        print parserSpec, " ==> ", results
    except ParseException, pe:
        print "ERROR Creating Grammar:", parserSpec, " ==> ", pe
        exit()
    else:
        print "PARSING SUCCESS!\n"
        for STRCT in results:
			print "struct ",STRCT[1],"="
			TraverseParseElement(STRCT[3], "    ")

    parserGlobalText = r"""

const int bufmax = 32*1024;
#define streamEnd (stream->eof() || stream->fail())
#define ChkNEOF {if(streamEnd) {flags|=fileError; statusMesg="Unexpected End of file"; break;}}
#define getbuf(cursor, c) {ChkNEOF; for(p=0;(c) && !streamEnd ;buf[p++]=streamGet(cursor)){if (p>=bufmax) {flags|=fileError; statusMesg="String Overflow"; break;}} buf[p]=0;}

#define U8_IS_SINGLE(c) (((c)&0x80)==0)

UErrorCode err=U_ZERO_ERROR;
const UnicodeSet TagChars(UnicodeString("[[:XID_Continue:]']"), err);
const UnicodeSet TagStarts(UnicodeString("[:XID_Start:]"), err);
bool iscsymOrUni (char nTok) {return (iscsym(nTok) || (nTok&0x80));}
bool isTagStart(char nTok) {return (iscsymOrUni(nTok)&&!isdigit(nTok)&&(nTok!='_'));}
bool isAscStart(char nTok) {return (iscsym(nTok)&&!isdigit(nTok));}
bool isBinDigit(char ch) {return (ch=='0' || ch=='1');}
const icu::Normalizer2 *tagNormer=Normalizer2::getNFKCCasefoldInstance(err);

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

        func: ~infonParser(){delete punchInOut;};   END

        func: char streamGet(streamSpan cursor){
            char ch;
            switch(flags&(fullyLoaded|userMode)){
                case(userMode):  // !fully loaded | userMode
                    while(cursor.offset > textStreamed.size()){
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
                    textStreamed.resize(stream->tellg());
                    stream->seekg(0, std::ios::beg);
                    stream->read((char*)textStreamed.data(), textStreamed.size());
                    flags|=fullyLoaded;
                    break;
            }
            return textStreamed[cursor.offset];
        };  END

        func: void scanPast(streamSpan cursor, char* str){
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

        func: bool chkStr(streamSpan cursor, const char* tok){
            int startPos=cursor.offset;
            if (tok==0) return 0;
            for(const char* p=tok; *p; p++) {
                if ((*p)==' ') RmvWSC(cursor);
                else if ((*p) != streamGet(cursor)){
                    cursor.offset=startPos;
                    return false;
                }
            }
            return true;
        };           END

        func: void streamPut(int nChars){for(int n=nChars; n>0; --n){stream->putback(textStreamed[textStreamed.size()-1]); textStreamed.resize(textStreamed.size()-1);}}; END

        func: const char* nxtTokN(streamSpan cursor, int n, ...){
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

        func: const char* nxtTok(streamSpan cursor, string s){return nxtTokN(cursor, 1, s.data());};           END

        func: char pPeek(streamSpan cursor){
            while(textStreamed.length() < cursor.offset){
                if(flags&fullyLoaded){return 0;}
                char ch=stream->get();
                if(stream->eof()){}
                if(stream->fail()){}
                textStreamed + ch;
            }
            return(textStreamed[cursor.offset]);
        } END

        func: char WSPeek(streamSpan cursor){RmvWSC(cursor); return pPeek(cursor);} END

        func: void RmvWSC(streamSpan cursor, attrStorePtr attrs=0){
            char p,p2;
            for (p=pPeek(cursor); (p==' '||p=='/'||p=='\n'||p=='\r'||p=='\t'||p=='%'); p=pPeek(cursor)){
                if (p=='/') {
                    streamGet(cursor); p2=pPeek(cursor);
                    if (p2=='/') {
        //              punchInOut->push_back(textStreamed.size()-1);  // Record start of line comment.
                        string comment="";
                        for (p=pPeek(cursor); !streamEnd && p!='\n'; p=pPeek(cursor)) comment+=streamGet(cursor);
        //                punchInOut->push_back(-textStreamed.size());  // Record end of line comment.
                        if (attrs){
                            if     (comment.substr(1,7)=="author=") attrs->a.insert(pair<string,string>("author",comment.substr(8)));
                            else if(comment.substr(1,6)=="image=") attrs->a.insert(pair<string,string>("image",comment.substr(7)));
                            else if(comment.substr(1,9)=="engTitle=") attrs->a.insert(pair<string,string>("engTitle",comment.substr(10)));
                            else if(comment.substr(1,7)=="import=") attrs->a.insert(pair<string,string>("import",comment.substr(8)));
                            else if(comment.substr(1,8)=="summary=") attrs->a.insert(pair<string,string>("summary",comment.substr(9)));
                            else if(comment.substr(1,5)=="link=") attrs->a.insert(pair<string,string>("link",comment.substr(6)));
                            else if(comment.substr(1,9)=="category=") attrs->a.insert(pair<string,string>("category",comment.substr(10)));
                            else if(comment.substr(1,7)=="posted=") attrs->a.insert(pair<string,string>("posted",comment.substr(8)));
                            else if(comment.substr(1,8)=="updated=") attrs->a.insert(pair<string,string>("updated",comment.substr(9)));
                        }
                    } else if (p2=='*') {
                        punchInOut->push_back(textStreamed.size()-1);  // Record start of block comment.
                        for (p=streamGet(cursor); !streamEnd && !(p=='*' && pPeek(cursor)=='/'); p=streamGet(cursor))
                            if (p=='\n') ++line;
                        if (streamEnd) throw "'/*' Block comment never terminated";
                        streamGet(cursor);
                        punchInOut->push_back(-(textStreamed.size()));  // Record end of block comment.
                    } else {streamPut(1); return;}
                } else if (p=='%'){
                    streamGet(cursor); p2=pPeek(cursor);
                    if(p2=='>'){scanPast(cursor, (char*)"<%");} else {streamPut(1); return;}
                }
                if (streamGet(cursor)=='\n') {++line; prevChar='\n';} else prevChar='\0';
            }
        };      END

        func: bool parseFlagSimple(streamSpan *predCursor, streamSpan *cursor, string token, uint64_t &flag, int bitPos){ // True if no errors
            cursor->offset=predCursor->offset+predCursor->length; cursor->length=0;
            char* tokFound = nxtTok(cursor, token);
            if(flags&(fileError|parseError)){ return false; }
            if(token==tokFound){
                flag |= (1<<bitPos);
                return true;
            }

        } END

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


        func: bool doRule(infon* i){
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
    FillStructFromText(ParserStructsName, parserFields)
    structNames.append(ParserStructsName)
    
#############################################    L o a d / P a r s e   P r o g r a m   S p e c

def bitsNeeded(n):
    if n <= 1:
        return 0
    else:
        return 1 + bitsNeeded((n + 1) / 2)


f=open(definitionFile)
d = f.read()
f.close()

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
modifierCmds = re.split("|", dataTags['modifierCmds'])
for modifierCmd in modifierCmds:
    print "MODIFIER: ",modifierCmd
    exec modifierCmd


#############################################   C r e a t e   A u x i l a r y   C o d e: enums, #defines, etc

structForwardDecls="\n";
structEnums="\n///////////////////////////////////////////////////\n////   E N U M E R A T I O N S \n\n"
structCode =""      # for storing the struct section of generated code.
structPtrs = {}
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
#			addField(struct, 'var', field['fieldType'], fieldName)
            #if(len(field)==3): field.append(0)
            structsData[struct]["fields"].append([field['fieldType'], fieldName])
        elif kindOfField=='ptr':
            #if(len(field)==3): field.append(0)
            structsData[struct]["fields"].append([field['fieldType']+"Ptr", fieldName])
            structPtrs[field['fieldType']]=field['fieldType'];
        elif kindOfField=='func':
            structsData[struct]["fields"].append(["FUNC", fieldName, field['funcText']])
        else: print "\nWARNING!!! Invalid field type!\n";

    structsList.append(struct)

#############################################    S y n t h e s i s   P a s s

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
            structCode += "    "+structField[2]+"\n\n"
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
headerFile += structForwardDecls;
headerFile += dataTags['global'] + parserGlobalText

headerFile += "\nstruct infSource{\n    uint32_t offset, length;\n};\n"
headerFile += structEnums+"\n"+structPtrCodeTop+"\n" + structCode +"\n" + structPtrCodeEnd +"\n\n"
headerFile += parserString
headerFile += '\n'

#print (headerFile)

fo=open(dataTags['FileName'], 'w')
fo.write(headerFile)
fo.close()
