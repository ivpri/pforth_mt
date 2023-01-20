/* pforth_mt custom words and exception approach */

#undef W
#undef WI
#undef TC

#ifdef PFCUSTOM_CODE
#define W(id, name, code)  \
        case ID_C ## id: { \
            code; } endcase;
#define WI(id, name, code) W(id, name, code)
#define TC(n, id, text)  WC(id, #id, n)
#undef PFCUSTOM_CODE

#elif defined PFCUSTOM_IDS
#define W(id, name, code) ID_C ## id,
#define WI(id, name, code) W(id, name, code)
#define TC(n, id, text)  WC(id, #id, n)
#undef PFCUSTOM_IDS

#elif defined PFCUSTOM_DICT
#define W(id, name, code)  CreateDicEntryC( ID_C ## id, name, 0 );
#define WI(id, name, code) CreateDicEntryC( ID_C ## id, name, FLAG_IMMEDIATE );    
#define TC(n, id, text)  WC(id, #id, n)
#undef PFCUSTOM_DICT

#else
#define W(id, name, code)
#define WI(id, name, code)

#ifdef PFCUSTOM_THROW_IDS
#define TC(n, id, text)  id = n,
#undef PFCUSTOM_THROW_IDS

#elif defined PFCUSTOM_THROW_TEXT
#define TC(n, id, text)  case id: s = text; break;
#undef PFCUSTOM_THROW_TEXT

#else
#define TC(n, id, text)
#endif

#endif



#ifndef W1

#define TO_TOS(x) TOS = (cell_t) (x)

#define WC( id, name, lit)        W(id, name, PUSH_TOS; TOS = (cell_t) (lit))
#define WC2( id, name, lit1, lit2) W(id, name, PUSH_TOS; M_PUSH(lit1); TOS = (cell_t) (lit2))

#define W11( id, name, p1, code)     W(id, name, cell_t p1 = TOS; code)
#define WI11(id, name, p1, code)    WI(id, name, cell_t p1 = TOS; M_DROP; code)
#define W21( id, name, p1, p2, code) W(id, name, \
    cell_t p2 = TOS;                             \
    cell_t p1 = M_POP; code)

#define WI21(id, name, p1, p2, code) WI(id, name, \
    cell_t p2 = TOS;                              \
    cell_t p1 = M_POP; code)

#define W31( id, name, p1, p2, p3, code) W(id, name, \
    cell_t p3 = TOS;                                 \
    cell_t p2 = M_POP;                               \
    cell_t p1 = M_POP; code)

#define WI31(id, name, p1, p2, p3, code) WI(id, name, \
    cell_t p3 = TOS;                                  \
    cell_t p2 = M_POP;                                \
    cell_t p1 = M_POP; code)

#define W41( id, name, p1, p2, p3, p4, code) W(id, name, \
    cell_t p4 = TOS;                                     \
    cell_t p3 = M_POP;                                   \
    cell_t p2 = M_POP;                                   \
    cell_t p1 = M_POP; code)

#define WI41(id, name, p1, p2, p3, p4, code) WI(id, name, \
    cell_t p4 = TOS;                                      \
    cell_t p3 = M_POP;                                    \
    cell_t p2 = M_POP;                                    \
    cell_t p1 = M_POP; code)

#define W51( id, name, p1, p2, p3, p4, p5, code) W(id, name, \
    cell_t p5 = TOS;                                         \
    cell_t p4 = M_POP;                                       \
    cell_t p3 = M_POP;                                       \
    cell_t p2 = M_POP;                                       \
    cell_t p1 = M_POP; code)

#define WI51(id, name, p1, p2, p3, p4, p5, code) WI(id, name, \
    cell_t p5 = TOS;                                          \
    cell_t p4 = M_POP;                                        \
    cell_t p3 = M_POP;                                        \
    cell_t p2 = M_POP;                                        \
    cell_t p1 = M_POP; code)

#define W61( id, name, p1, p2, p3, p4, p5, p6, code) W(id, name, \
    cell_t p6 = TOS;                                         \
    cell_t p5 = M_POP;                                       \
    cell_t p4 = M_POP;                                       \
    cell_t p3 = M_POP;                                       \
    cell_t p2 = M_POP;                                       \
    cell_t p1 = M_POP; code)

#define WI61(id, name, p1, p2, p3, p4, p5, p6, code) WI(id, name, \
    cell_t p6 = TOS;                                          \
    cell_t p5 = M_POP;                                        \
    cell_t p4 = M_POP;                                        \
    cell_t p3 = M_POP;                                        \
    cell_t p2 = M_POP;                                        \
    cell_t p1 = M_POP; code)

#define W1(  id, name, p1, code)  W11( id, name, p1, M_DROP; code)
#define WI1( id, name, p1, code)  WI11(id, name, p1, M_DROP; code)
#define W2(  id, name, p1, p2, code)  W21( id, name, p1, p2, M_DROP; code)
#define WI2( id, name, p1, p2, code)  WI21(id, name, p1, p2, M_DROP; code)
#define W3(  id, name, p1, p2, p3, code)  W31( id, name, p1, p2, p3, M_DROP; code)
#define WI3( id, name, p1, p2, p3, code)  WI31(id, name, p1, p2, p3, M_DROP; code)
#define W4(  id, name, p1, p2, p3, p4, code)  W41( id, name, p1, p2, p3, p4, M_DROP; code)
#define WI4( id, name, p1, p2, p3, p4, code)  WI41(id, name, p1, p2, p3, p4, M_DROP; code)
#define W5(  id, name, p1, p2, p3, p4, p5, code)  W51( id, name, p1, p2, p3, p4, p5, M_DROP; code)
#define WI5( id, name, p1, p2, p3, p4, p5, code)  WI51(id, name, p1, p2, p3, p4, p5, M_DROP; code)
#define W6(  id, name, p1, p2, p3, p4, p5, p6, code)  W61( id, name, p1, p2, p3, p4, p5, p6, M_DROP; code)
#define WI6( id, name, p1, p2, p3, p4, p5, p6, code)  WI61(id, name, p1, p2, p3, p4, p5, p6, M_DROP; code)

#define W1T(  id, name, p1, code)  W11( id, name, p1, TO_TOS(code))
#define WI1T( id, name, p1, code)  WI11(id, name, p1, TO_TOS(code))
#define W2T(  id, name, p1, p2, code)  W21( id, name, p1, p2, TO_TOS(code))
#define WI2T( id, name, p1, p2, code)  WI21(id, name, p1, p2, TO_TOS(code))
#define W3T(  id, name, p1, p2, p3, code)  W31( id, name, p1, p2, p3, TO_TOS(code))
#define WI3T( id, name, p1, p2, p3, code)  WI31(id, name, p1, p2, p3, TO_TOS(code))
#define W4T(  id, name, p1, p2, p3, p4, code)  W41( id, name, p1, p2, p3, p4, TO_TOS(code))
#define WI4T( id, name, p1, p2, p3, p4, code)  WI41(id, name, p1, p2, p3, p4, TO_TOS(code))
#define W5T(  id, name, p1, p2, p3, p4, p5, code)  W51( id, name, p1, p2, p3, p4, p5, TO_TOS(code))
#define WI5T( id, name, p1, p2, p3, p4, p5, code)  WI51(id, name, p1, p2, p3, p4, p5, TO_TOS(code))
#define W6T(  id, name, p1, p2, p3, p4, p5, p6, code)  W51( id, name, p1, p2, p3, p4, p5, p6, TO_TOS(code))
#define WI6T( id, name, p1, p2, p3, p4, p5, p6, code)  WI51(id, name, p1, p2, p3, p4, p5, p6, TO_TOS(code))

#endif