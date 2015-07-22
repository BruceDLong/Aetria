
    BuildCmd: `g++ -std=gnu++11 core.cpp -I/home/bruce/proteus/xbuilds/localHost/builds/libs/unicode/release-51-1/include/ -L/home/bruce/proteus/xbuilds/localHost/builds/libs/unicode/release-51-1/lib/ -pthread   -licuio -licui18n -licutu -licuuc -licudata -licule -liculx -licutest -ldl  -o core`
    Title: `Proteus Definition`
    FileName: `Prot.h`
    Version: `1`

    Platform1:`amd64:linux:c++`

    Include: `<cstdint>, <string>, <cstring>, <vector>, <map>, <cstdarg>, "remiss.h", <boost/intrusive_ptr.hpp>,
            "unicode/locid.h", "unicode/unistr.h", "unicode/putil.h", "unicode/ustream.h", "unicode/uspoof.h", "unicode/unorm.h", "unicode/normalizer2.h"`

    modifierCmds: `generateParser(dataTags['infonParse'], 'infon')`


    infonParse: `{
        StructParser pureInfon = { !"asd" "jui" #infon
        }


        StructParser infon = { WS
            [( "@X"; toExec = true;) | (!"@X"; toExec = false;)] WS
            [("@";  asDesc = true;) | (!"@";  asDesc = false;)] WS
            [("!";  asNot  = true;) | (!"!";  asNot  = false;)] WS
            [
                ("?";  value.type=tUnknown; value.format=fUnknown; size.type=tNum; size.format=fUnknown;)
                | ("%W"; FindMode=iToWorld;)
                | ("%C"; FindMode=iToCtxt; )
                | ("%A"; FindMode=iToArgs; )
                | ("%V"; FindMode=iToVars; )
                | ("%\\";FindMode=iToHome; )
                | #tagChain
                | ({"*" #size "+" #value};)
                | (size=1; {"+" #value}; )
                | (size.type=tNum; size.format=fUnknown; #value; )
            ] WS

            {
                [!":" | "::" | ":"]
                ["==" | "="]
                [!":" | "::" | ":"]
                #infon
            } WS
            {
                [!"<" | {"<:" #infon} | {"<!" #infon}] WS
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

    struct infNode {
        flag:    ok
        ptr:     infon    item
        ptr:     infNode  next
    }

    struct pureInfon {
        flag: Initialized
        mode: ParseState [pUnset, notParsed, parsing, parsed]
        flag: LeftEdgeReadyToParse

        mode: Type      [tUnknown, tNum, tStr, tList]
        mode: Format    [fUnknown, fLiteral, fConcat, fReel, fFraction]
        mode: NumBase   [dDefault, dDecimal, dHex, dBinary]
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
        flag: asNot
        flag: xOptimized

        var: pureInfon size;
        var: pureInfon value;

        ptr: infon pred
        ptr: infon args

        var: infNode workList;
        var: streamSpan curPos;

        func: bool doRule(infonPtr i){
        }
        END
    }



    struct agent{
        mode: coreRunState [corePreInit, coreParsing, coreRunning]
        var: string resourceDir;
        var: string dbName;
        var: string newsURI;
        var:  infon topInfon;
        var:  infonParser parser;
        var:  bool doneYet;

        func: bool doRule(infon* i){
            switch(i->flags&coreRunStateMask){
                case corePreInit:    break;
                case coreParsing: parser.doRule(i);    break;
                case coreRunning: topInfon.doRule(i);  break;
                default:exit(2);
            }
        } END

        func: bool pollEvent(infon** inf){return 0;}END
        func: int eventLoop(){
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

    }


}
