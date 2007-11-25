! English menus for Varkon 2.0

m1  = "VARKON-DRAFTING"
      "file",m5
      "create",m11
      "edit",m49
      "view",m67
      "delete",m80
      "analysis",m75
      "settings",m9
      "help",m22;

m2  = "VARKON-GENERIC"
      "file",m5
      "create",m11
      "edit",m15
      "view",m67
      "analysis",m75
      "settings",m9
      "help",m22;

m5  = "FILE"
      "new/open",m47
      "save",f175
      "save as",m45
      "export",m24
      "macro",f152
      "shell",f125
      "exit",f123
      "save/exit",f122;

m7  = "PROJECT"
      "new project",f176
      "project settings",f178
      "delete project",f179;

m9  = "SETTINGS"
      "curve accuracy",f98
      "attributes",f16
      "module",f127
      "project",m7;

m10 = "TCOPY"
      "translation",f239
      "rotation",f240
      "mirror",f241
      "copy",f242;

m11 = "CREATE"
      "point",m20
      "line",m40
      "arc",m55
      "curve",m60
      "surface",m12
      "coordsys",m25
      "text",m23
      "dimension",m50
      "tcopy",m10
      "group",f55
      "part",f25
      "import",m39;

m12 = "SURFACE"
      "bplane",f26
      "rotational",f158
      "cylinder",f160
      "sweep",f181
      "ruled",f182
      "lofted",f238
      "offset",f159
      "approx",f235
      "trimmed",f234
      "composite",f236
      "import",f237;

m15 = "EDIT"
      "pen",f11
      "level",f12
      "linewidth",f185
      "attribute",m16
      "entity ",M117
      "MBS-statement",f168
      "MBS edit part",f217
      "compile all",f220
      "update part",f109
      "trim",f6;

m16 = "ATTRIBUTE"
      "line",m17
      "arc",m17
      "curve",m90
      "text",m18
      "dimension",m19
      "hatch",m17;

m17 = "ATTRIBUTE"
      "solid",f74
      "dashed",f76
      "centre",f77
      "dash length",f73;

m18 = "TEXT"
      "height",f78
      "width",f79
      "slant",f89
      "font",f171
      "tpmode",f226;

m19 = "DIMENSION"
      "text size",f154
      "arrow size",f155
      "decimals",f129
      "auto",f131
      "no auto",f130;

m20 = "POINT"
      "free",f19
      "projected",f21;

m22 = "HELP"
      "about Varkon",M184
      "help",f153;

m23 = "TEXT"
      "left",f50
      "right",M113
      "centre   ",M114
      "2 positions",M115;
!      "font-editor",m93;

m24 = "EXPORT"
      "MBS",f144
      "DXF-2D",f92
      "DXF-3D",M398
      "STL",M1625
      "PLT",f151;

m25 = "COORDSYS"
      "3 pos",f105
      "pos/angles",f9;

m30 = "LINEAR DIMENSION"
      "horizontal",f30
      "vertical ",f31
      "parallell",f32;

m33 = "DIAMETER DIMENSION"
      "horizontal",f33
      "vertical ",f34
      "parallell",f35;

m39 = "IMPORT"
      "IGES",p1624
      "DXF-2D",p388
      "DXF-3D",p389
      "STL",p1623
      "PLT",f145;

m40 = "LINE"
      "2f two pos",f22
      "2p projected",f41
      "pl parallell",f23
      "pr perpendicular",f54
      "pa pos/angle ",f24
      "pt pos/tan",f28
      "t  tan/tan",f29;

m45 = "SAVE AS"
      "all files",f201
      "JOB-file",f119
      "MBO-file",f126
      "MBS-file",f144
      "RES-file",f121;

m47 = "NEW/OPEN"
      "save first",f124
      "don't save",f120;

m49 = "EDIT"
      "edit",m81
      "trim",f6
      "drag",f202
      "move",f209
      "mirror",f211
      "rotate",f203
      "copy",f210;

m50 = "DIMENSION"
      "linear",m30
      "diameter",m33
      "radius",f36
      "angle",f37
      "hatch",f38;

m55 = "ARC"
      "1 pos/angles",f56
      "2 pos/radius",f57
      "3 pos",f58
      "offset",f59
      "fillet",f65;

m60 = "CURVES"
      "spline",m89
      "conic",m84
      "offset",f163
      "composite",f75
      "trimmed",f232
      "isoparam",f230
      "intersect",f183
      "silhouette",f233
      "approx",f231;

m62 = "HIDE"
      "window",f133
      "file",f134
      "both",f135;

m63 = "FERGUSON"
      "with tangents",f70
      "no tangents",f116;

m64 = "CHORD LENGTH"
      "with tangents",f71
      "no tangents",f156;

m67 = "VIEW"
      "colors",M1658
      "all hitable",f215
      "X11 window",f196
      "OpenGL window",f200
      "hide",m62;

m75 = "ANALYSIS"
      "formula",M393
      "geometry",m76 
      "part/group",m79
      "job status",M368
      "system",m77;

m76 = "GEOMETRY"
      "one entity",M356
      "position",M363
      "distance",M364
      "angle",M365
      "curves",m91
      "surfaces",M1643
      "area",m86
      "cgrav",M417;

m77 = "SYSTEM"
      "DB-data",f140
      "PM-data",f139;

m79 = "PART/GROUP"
      "one part",M379
      "call chain",M418
      "references",f169
      "part members",M366
      "list parts",M378
      "part tree",p419
      "gm-list",M420
      "group members",M360
      "list groups",M361;

m80 = "DELETE"
      "entity",f10
      "group",f13
      "blank",f213
      "unblank all",f214;

m81 = "EDIT"
      "pen",f11
      "level",f12
      "linewidth",f185
      "attribute",m16
      "arc",m82
      "text",m83
      "group",f212
      "part",m35;

m82 = "ARC"
      "radius",f204
      "start angle",f205
      "end   angle",f206;

m83 = "TEXT"
      "angle",f207
      "text",f208;

m84 = "CONIC"
      "free",f161
      "projected",f162;

m86 = "AREA"
      "2D-area",M416
      "section",M1591
      "surface",M1589;

m89 = "SPLINE"
      "ferguson",m63
      "chord length",m64
      "variable stiffness",m97;

m90 = "CURVE ATTRIBUTE"
      "solid",f74
      "dashed",f76
      "centre",f77
      "filled",f218
      "dash length",f73;

m91 = "CURVES"
      "length",M431
      "tangent",M432
      "normal",M434
      "bi-normal",M435
      "curvature",M433
      "inverse",m92;

m92 = "INVERSE"
      "position",M436
      "tangent",M437
      "curvature",M438
      "length",M439;

m93 = "FONT_EDITOR"
      "template",p176
      "points ",f20
      "draw",M177
      "move",p178
      "genererate file",m94;

m94 = "GENERERATE FILE"
       "this char",M179
       "all  char",M180;

m97 = "VAR STIFFNESS"
      "with tangents",f72
      "no tangents",f157;

m100 = "2 POSITIONS"
       "1 startpos",a1
       "2 endpos",a2;

m101 = "PARALLELL"
       "line",a1
       "distance",a2;

m102 = "POS-ANGLE"
       "startpos",a1
       "angle",a2
       "length",a3;

m103 = "PERPENDICULAR"
       "startpos",a1
       "line",a2
       "length",a3;

m104 = "POS-TANGENT"
       "startpos",a1
       "arc_curve",a2
       "alternative",a3;

m105 = "TAN-TAN"
       "first arc",a1
       "second arc",a2
       "alternative",a3;

m106 = "1 POSITION"
       "origin",a1
       "radius",a2
       "startangle",a3
       "endangle",a4;

m107 = "TWO POS"
       "1 startpos",a1
       "2 endpos",a2
       "radius",a3;

m108 = "THREE POS"
       "1 startpos",a1
       "2 midpos",a2
       "3 endpos",a3;

m109 = "FILLET"
       "1:st entity",a1
       "2:nd entity",a2
       "radius",a3
       "alternative",a4;

m110 = "OFFSET"
       "arc",a1
       "distance",a2;

m111 = "LINEAR DIM"
       "1 startpos",a1
       "2 endpos",a2
       "textpos",a3
       "alternative",a4;

m112 = "CIRCULAR DIM"
       "arc",a1
       "textpos",a2
       "alternative",a3;

m113 = "RADIUS DIM"
       "arc",a1
       "breakpos",a2
       "textpos",a3;

m114 = "ANGULAR DIM"
       "1:st line",a1
       "2:nd line",a2
       "textpos",a3
       "alternative",a4;

m115 = "TEXT"
       "position",a1
       "direction",a2
       "text",a3;

m116 = "HATCH"
       "distance",a1
       "angle",a2;

m117 = "CSYS 1P"
       "name",a1
       "origin",a2
       "x-angle",a3
       "y-angle",a4
       "z-angle",a5;

m118 = "KSYS 3P"
       "name",a1
       "origin",a2
       "x-axis",a3
       "y-axis",a4;

m119 = "B_PLANE"
       "1:st corner",a1
       "2:nd corner",a2
       "3:rd corner",a3
       "4:th corner",a4;

m120 = "CONIC"
       "1 startpos",a1
       "2 starttang",a2
       "3 P-value",a3
       "4 endpos",a4
       "5 endtang",a5;

m121 = "OFFSET"
       "curve",a1
       "offset",a2;

m122 = "SPLINE"
       "1 startpos",a1
       "2 starttang",a2
       "3 endpos",a3
       "4 endtang",a4;

m123 = "INTERSECT"
       "surface",a1
       "plane",a2;

m124 = "ISOPARAM"
       "surface",a1
       "type",a2
       "iso-value",a3
       "start",a4
       "end",a5;

m125 = "APPROX"
       "curve",a1
       "type",a2
       "method",a3
       "data" ,a4;

m126 = "TRIM"
       "curve",a1
       "start",a2
       "end" ,a3;

m127 = "SILHOUETTE"
       "surface",a1
       "direction",a2
       "type" ,a3
       "branch" ,a4;

m134 = "GROUP"
       "name",a3
       "delete",a4
       "add",a5;

m140 = "POSITION"
       "absolute",a1
       "relative",a2
       "cursor",a3
       "end",a4
       "on",a5
       "centre",a6
       "intersect",a7;

m141 = "POSITION"
       "absolute",a1
       "relative",a2
       "cursor",a3
       "mbs",a4
       "end",a5
       "on",a6
       "centre",a7
       "intersect",a8;

m143 = "PARAMETER TYPE"
       "integer",a1
       "float",a2
       "string",a3
       "vector",a4;

m144 = "MODULE TYPE"
       "geometry 3D",a1
       "drawing  2D",a2;

m145 = "ATTRIBUTE"
       "local",a1
       "global",a2
       "basic",a3;

m146 = "CREATE PLOT FILE"
       "entire model",a1
       "entire window",a2
       "part of window",a3
       "another view",a4;

m147 = "CREATE DXF FILE"
       "entire model",a1
       "entire window",a2
       "part of window",a3
       "another view",a4;

m150 = "OFFSET"
       "surface",a1
       "offset",a2;

m151 = "ROTATION"
       "curve",a1
       "start angle",a2
       "end angle",a3
       "1:st position",a4
       "2:nd position",a5;

m152 = "CYLINDER"
       "curve",a1
       "length",a2
       "reverse",a3;

m153 = "SWEEP"
       "profile",a1
       "control",a2
       "reverse",a3;

m154 = "RULED"
       "1:st curve",a1
       "2:nd curve",a2
       "reverse",a3;

m155 = "TRIM"
       "surface",a1
       "u start",a2
       "u end"  ,a3
       "v start",a4
       "v end"  ,a5;

m156 = "IMPORT"
       "file name",a1
       "file type",a2
       "surface type",a3;

m157 = "SUR_CONIC"
       "1 start curve",a1
       "2 start tangent",a2
       "3 P-value",a3
       "4 end curve",a4
       "5 end tangent",a5;
