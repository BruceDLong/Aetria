
    BuildCmd: `g++ -std=gnu++11 core.cpp -I/home/bruce/proteus/xbuilds/localHost/builds/libs/unicode/release-51-1/include/ -L/home/bruce/proteus/xbuilds/localHost/builds/libs/unicode/release-51-1/lib/ -pthread   -licuio -licui18n -licutu -licuuc -licudata -licule -liculx -licutest -ldl  -o core`
    Title: `Proteus Definition`
    FileName: `Prot.cpp`
    Version: `1`

    Platform1:`amd64:linux:c++`

    Include: `<fstream>, <cstdint>, <string>, <cstring>, <vector>, <map>, <cstdarg>, "remiss.h", <boost/intrusive_ptr.hpp>,
            "unicode/locid.h", "unicode/unistr.h", "unicode/putil.h", "unicode/ustream.h", "unicode/uspoof.h", "unicode/unorm.h", "unicode/normalizer2.h"`

    modifierCmds: `
        generateParser(dataTags['infonParse'], 'infon')
        | generateEventHandler()
        | generateMainFunction()
    `


    infonParse: `{
        StructParser pureInfon = {pureInfon !"asd" "jui"
        }

        StructParser infNode = {is
            [leftIs (!":"  <=>  leftMode=colLeft0; ) |  ("::" <=>  leftMode=colLeft2; ) | ( ":"<=> leftMode=colLeft1; )]
            [midIs ("==" <=> looseType=true;) | ("=" <=> looseType=false;) ]
            [rightIs (!":"  <=>  rightMode=colRight0; ) |  ("::" <=>  rightMode=colRight2; ) | ( ":"<=> rightMode=colRight1; )]
            #item
            <#next>
            WS
        }

        StructParser infon = {infon WS
            [toExec ("@X" <=> toExec = true;)  | (!"@X"<=>  toExec = false;)]  WS
            [asDesc ("@"  <=> asDesc = true;)  | (!"@" <=>  asDesc = false;)]  WS
            [toHome ("\\"  <=> moveHome = true;) | (!"\\" <=>  moveHome = false;)] WS       // Move the cursor to home
            [asNot  ("!"  <=> asNot  = true;)  | (!"!" <=>  asNot  = false;)]  WS
            [sizeAndValue
                ("?" <=>   value.type=tUnknown; value.format=fUnknown; size.type=tNum; size.format=fUnknown;)
            //    | ()
                | ("%W"<=>  FindMode=iToWorld;)
                | ("%C"<=>  FindMode=iToCtxt; )
                | ("%A"<=>  FindMode=iToArgs; )
                | ("%V"<=>  FindMode=iToVars; )
                | ("%\\"<=> FindMode=iToPathH;)
             //   | #tagChain
                | ({sizeAndValueLiteral  "*" #size "+" #value} <=>  FindMode=iNone;)
                | ({Value "+" #value} <=>  size=1; )
                | (#value <=>  size.type=tNum; size.format=fUnknown; )
            ] WS

            <#workList>

            {args
                [noFunc !"<" | {Func "<:" #args} | {negFunc "<!" #args}] WS
            }
        }
    }`

    global:`

`

    struct streamSpan {
        var: uint32_t offset;
        var: uint32_t length;
    }

    struct posRec{
        flag: isStart
        flag: isEnd
        ptr:  infon infParsed;
    };
    struct posRecStore{
        var: string s;
        var: map<uint, posRec>  ps;
        var ColorCodes* colorCodes;
    };

    struct attrStore{
        var: map<string, string> a;
    }

    struct pureInfon {
        flag: Initialized
        mode: ParseState [pUnset, notParsed, parsing, parsed]
        flag: LeftEdgeReadyToParse

        mode: type      [tUnknown, tNum, tStr, tList]
        mode: format    [fUnknown, fLiteral, fConcat, fReel, fFraction]
        mode: numBase   [dDefault, dDecimal, dHex, dBinary]
        flag: hasPattern
        flag: invert
        flag: embeddedSeq
    }


    struct infon {
        flag: InfInitialized
        mode: InfParseState [iUnset, iNotParsed, iParsing, iParsed]
        mode: InfParsePhase [iStartParse, iParseIdents, iParseFuncArgs]
        flag: InfLeftEdgeReadyToParse

        mode: FindMode [iNone, iToWorld, iToCtxt, iToArgs, iToVars, iToPath, iToPathH, iTagUse, iTagDef, iHardFunc, iGetSize, iGetType, iAssocNxt, iGetLast, iGetFirst, iGetMiddle]

        flag: asDesc
        flag: toExec
        flag: moveHome
        flag: asNot
        flag: xOptimized

        var: pureInfon size;
        var: pureInfon value;

        ptr: infon pred
        ptr: infon args

        ptr: infNode workList
        var: streamSpan curPos;

        func: bool: doRule(infonPtr i){
        }
        END
    }

    struct infNode {
        flag:    looseType
        mode:    leftMode  [colLeft0,  colLeft1,  colLeft2]
        mode:    rightMode [colRight0, colRight1, colRight2]
        ptr:     infon    item
        ptr:     infNode  next
    }






}
