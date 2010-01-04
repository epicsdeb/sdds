(* Mathematica Routines for SDDS *)

editsdds:=Run["emacs sdds.m"]

BeginPackage["SDDS`"]

SDDSRead::usage=
    "SDDSRead[filename_String] returns an SDDS structure from a file.";
SDDSWrite::usage=
    "SDDSWrite[sdds_,filename_String] writes an SDDS structure to a file.";
SDDSGetColumnDefinitions::usage=
    "SDDSGetColumnDefinitions[sdds_] returns the list of column definitions.";
SDDSGetParameterDefinitions::usage=
    "SDDSGetParameterDefinitions[sdds_] returns the list of parameter definitions.";
SDDSGetArrayDefinitions::usage=
    "SDDSGetArrayDefinitions[sdds_] returns the list of array definitions.";
SDDSGetAssociates::usage=
    "SDDSGetAssociates[sdds_] returns the list of associates.";
SDDSGetTable::usage=
    "SDDSGetTable[sdds_,n_:1] returns the nth table {parameters,data}.";
SDDSGetParameters::usage=
    "SDDSGetParameters[sdds_,n_:1] returns the parameters from the nth table.";
SDDSGetParameter::usage=
    "SDDSGetParameter[sdds_,p_String,n_:1] returns the value of parameter p from the nth table.";
SDDSGetData::usage=
    "SDDSGetData[sdds_,n_:1] returns the data matrix from the nth table.";
SDDSGetColumn::usage=
    "SDDSGetColumn[sdds_,c_String,n_:1] returns the column named c from the nth table.\n"<>
    "SDDSGetColumn[sdds_,m_,n_:1] returns the mth column from the nth table.";
SDDSGetRow::usage=
    "SDDSGetRow[sdds_,m_,n_:1] returns the mth row from the nth table.";
SDDSGetNColumns::usage=
    "SDDSGetNColumns[sdds_] returns the number of columns.";
SDDSGetNParameters::usage=
    "SDDSGetNParameters[sdds_] returns the number of parameters.";
SDDSGetNArrays::usage=
    "SDDSGetNArrays[sdds_] returns the number of arrays.";
SDDSGetNAssociates::usage=
    "SDDSGetNAssociates[sdds_] returns the number of associates.";
SDDSGetNTables::usage=
    "SDDSGetNTables[sdds_] returns the number of tables.";
SDDSGetNRows::usage=
    "SDDSGetNRows[sdds_,n_:1] returns the number of rows in the nth table.";
SDDSGetColumnNames::usage=
    "SDDSGetColumnNames[sdds_] returns the list of column names.";
SDDSGetParameterNames::usage=
    "SDDSGetParameterNames[sdds_] returns the list of parameter names.";
SDDSGetArrayNames::usage=
    "SDDSGetArrayNames[sdds_] returns the list of array names.";
SDDSGetAssociateNames::usage=
    "SDDSGetAssociateNames[sdds_] returns the list of associate names.";

Begin["Private`"]

SDDSRead[filename_String]:=(
    Module[{sdds},
        sdds=Get[filename];
        If[sdds == $Failed,Print["SDDSRead failed on ",filename]];
        Return[sdds];
    ];
)

SDDSWrite[sdds_,filename_String]:=(
    Module[{strm},
        strm=OpenWrite[filename];
        Write[strm,sdds];
        Close[strm];
    ];
)

SDDSGetColumnDefinitions[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[coldef];
    ]
)

SDDSGetParameterDefinitions[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[pardef];
    ]
)

SDDSGetArrayDefinitions[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[arraydef];
    ]
)

SDDSGetAssociates[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[associates];
    ]
)

SDDSGetTable[sdds_,n_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[tables[[n]]];
    ]
)

SDDSGetParameters[sdds_,n_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[tables[[n,1]]];
    ]
)

SDDSGetParameter[sdds_,p_,n_:1]:=(
    Module[{parnames,ip},
        parnames=SDDSGetParameterNames[sdds];
        If[!MemberQ[parnames,p],
            Print["There is no parameter named ",p];
            Return[$Failed]            
        ];
        ip=1;
        While[parnames[[ip]] != p,ip++];
        Return[SDDSGetParameters[sdds,n][[ip]]];
    ]
)

SDDSGetData[sdds_,n_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[tables[[n,2]]];
    ]
)

SDDSGetColumn[sdds_,c_String,ntable_:1]:=(
    Module[{colnames,ic},
        colnames=SDDSGetColumnNames[sdds];
        If[!MemberQ[colnames,c],
            Print["There is no column named ",c];
            Return[$Failed]            
        ];
        ic=1;
        While[colnames[[ic]] != c,ic++];
        Return[SDDSGetColumn[sdds,ic,ntable]];
    ]
)

SDDSGetColumn[sdds_,ncol_,ntable_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Transpose[tables[[ntable,2]]][[ncol]]];
    ]
)

SDDSGetRow[sdds_,nrow_,ntable_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[tables[[ntable,2]][[nrow]]];
    ]
)

SDDSGetNColumns[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[coldef]];
    ]
)

SDDSGetNParameters[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[pardef]];
    ]
)

SDDSGetNArrays[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[arraydef]];
    ]
)

SDDSGetNAssociates[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[associates]];
    ]
)

SDDSGetNTables[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[tables]];
    ]
)

SDDSGetNRows[sdds_,n_:1]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        Return[Length[tables[[n,2]]]];
    ]
)

SDDSGetColumnNames[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables,n},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        n=Length[coldef];
        If[n > 0,
            Return[Table[coldef[[i,1]],{i,1,n}]];
            Return[{}]
        ];
    ]
)

SDDSGetParameterNames[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables,n},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        n=Length[pardef];
        If[n > 0,
            Return[Table[pardef[[i,1]],{i,1,n}]],
            Return[{}]
        ];
    ]
)

SDDSGetArrayNames[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables,n},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        n=Length[arraydef];
        If[n > 0,
            Return[Table[arraydef[[i,1]],{i,1,n}]],
            Return[{}]
        ];
    ]
)

SDDSGetAssociateNames[sdds_]:=(
    Module[{description,coldef,pardef,arraydef,associates,tables,n},
        {description,coldef,pardef,arraydef,associates,tables}=sdds;
        n=Length[associates];
        If[n > 0,
            Return[Table[associates[[i,2]],{i,1,n}]],
            Return[{}]
        ];
    ]
)

End[]

EndPackage[]

(* Emacs editing sequences follow: *)
(* Local Variables: *)
(* mode: text *)
(* tab-stop-list: (0 4 8 12 16 20 24 28 32 36 40 44 48 52 56) *)
(* indent-tabs-mode: nil *)
(* End: *)
