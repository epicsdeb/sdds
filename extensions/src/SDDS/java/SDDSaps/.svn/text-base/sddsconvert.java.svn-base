/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

//package SDDS.apps;

import java.lang.reflect.*;
import SDDS.java.SDDS.*;
/**
 * This program is used to convert SDDS files between ASCII and binary,
 * and to filter out unwanted columns and/or rows.<br><br>
 * usage: java sddsconvert [-pipe[={'input' | 'output'}]<br>
 * [&lt;source-file&gt;] [&lt;target-file&gt;]<br>
 * [{-binary | -ascii}]<br>
 * [-endianOut={'big'|'little'}] [-endianIn={'big'|'little'}]<br>
 * [-description=&lt;text&gt;,&lt;contents&gt;]<br>
 * [-fromPage=&lt;pageNumber&gt;] [-toPage=&lt;pageNumber&gt;]<br>
 * [-delete={'column' | 'parameter' | 'array'},&lt;matching-string&gt;[,...]]<br>
 * [-retain={'column' | 'parameter' | 'array'},&lt;matching-string&gt;[,...]]<br>
 * [-rename={'column' | 'parameter' | 'array'},&lt;oldname&gt;=&lt;newname&gt;[,...]]<br>
 * [-nowarnings]<br><br>
 * <table><tr><td>
 * -binary</td><td>
 * The output file will be written in binary format</td></tr><tr><td>
 * -ascii</td><td>
 * The output file will be written in ASCII format</td></tr><tr><td>
 * -endianOut</td><td>
 * The output file will be written in big or little endian</td></tr><tr><td>
 * -endianIn</td><td>
 * The input file will be read in big or little endian</td></tr><tr><td>
 * -description</td><td>
 * Add the text and contents to the description of the output file</td></tr><tr><td>
 * -fromPage</td><td>
 * First page in input file to write to output file</td></tr><tr><td>
 * -toPage</td><td>
 * Last page in input file to write to output file</td></tr><tr><td>
 * -delete</td><td>
 * Delete the column(s), parameter(s) or array(s) from output file</td></tr><tr><td>
 * -retain</td><td>
 * Retain the column(s), parameter(s) or array(s) in the output file</td></tr><tr><td>
 * -rename</td><td>
 * Rename the column(s), parameter(s) or array(s) in the output file</td></tr><tr><td>
 * -nowarnings</td><td>
 * Do not warn if existing file will be replaced</td></tr>
 * </table>
 *
 * @author    Robert Soliday 
 *            <a href=mailto:soliday@aps.anl.gov>
 *            soliday@aps.anl.gov</a>
 * @version   1.0 (November 23, 1999)
 * @since     JDK 1.2
 *
 * @version   1.1 (December 3, 2001)
 * @since     JDK 1.3
 */
class sddsconvert {
    static void printUsage() {
	String usage;
	usage = "usage: java sddsconvert [-pipe[={'input' | 'output'}]\n";
	usage = usage + "[<source-file>] [<target-file>]\n";
	usage = usage + "[{-binary | -ascii}]\n";
	usage = usage + "[-endianOut={'big'|'little'}] [-endianIn={'big'|'little'}]\n";
	usage = usage + "[-description=<text>,<contents>]\n";
	usage = usage + "[-fromPage=<pageNumber>] [-toPage=<pageNumber>]\n";
	usage = usage + "[-delete={'column' | 'parameter' | 'array'},<matching-string>[,...]]\n";
	usage = usage + "[-retain={'column' | 'parameter' | 'array'},<matching-string>[,...]]\n";
	usage = usage + "[-rename={'column' | 'parameter' | 'array'},<oldname>=<newname>[,...]]\n";
	usage = usage + "[-nowarnings]\n\n";
	usage = usage + "This program is used to convert SDDS files between ASCII and binary,\n";
	usage = usage + "and to filter out unwanted columns and/or rows.\n\n";
	usage = usage + "Program by Robert Soliday.";
	System.err.println(usage);
    }

    public static void main(String[] args) {
	String[][] commandLineArgs;
	String[] deleteColumns=null, deleteParameters=null, deleteArrays=null;
	String[] retainColumns=null, retainParameters=null, retainArrays=null;
	String[] columnNames=null, parameterNames=null, arrayNames=null;
	String[] renameColumns=null, renameParameters=null, renameArrays=null;
	String inputfile=null, outputfile=null;
	String descriptionText=null, descriptionContents=null;
	int arguments, elements, i, j, k, n;
	int fromPage=1, toPage=1;
	int pageCount;
	boolean changeMode = false;
	boolean binary = false;
	boolean changeEndian = false;
	boolean littleEndian = false;
	boolean changeEndianIn = false;
	boolean littleEndianIn = false;
	boolean changeDescription = false;
	boolean specifyToPage = false;
	boolean nowarnings = false;
	boolean pipeIn = false;
	boolean pipeOut = false;
	boolean found;

	commandLineArgs = SDDSUtil.parseCommandLine(args);
	arguments = Array.getLength(commandLineArgs);
	if (arguments==0) {
	  printUsage();
	  return;
	}
	for (i=0;i<arguments;i++) {
	    elements = Array.getLength(commandLineArgs[i]);
	    if (elements == 0)
		continue;
	    if (SDDSUtil.compare(commandLineArgs[i][0],"-binary")) {
		changeMode = binary = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-ascii")) {
		changeMode = true;
		binary = false;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-endianOut")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		changeEndian = true;
		if (SDDSUtil.compare(commandLineArgs[i][1],"little"))
		    littleEndian = true;
		else
		    littleEndian = false;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-endianIn")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		changeEndianIn = true;
		if (SDDSUtil.compare(commandLineArgs[i][1],"little"))
		    littleEndianIn = true;
		else
		    littleEndianIn = false;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-description")) {
		if (elements != 3) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		changeDescription = true;
		descriptionText = commandLineArgs[i][1];
		descriptionContents = commandLineArgs[i][2];
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-fromPage")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		try {
		    fromPage = (Integer.valueOf(commandLineArgs[i][1])).intValue();
		} catch (Exception e) {
		    System.err.println("-fromPage option nees a numerical value");
		    return;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-toPage")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		specifyToPage = true;
		try {
		    toPage = (Integer.valueOf(commandLineArgs[i][1])).intValue();
		} catch (Exception e) {
		    System.err.println("-toPage option nees a numerical value");
		    return;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-nowarnings")) {
		nowarnings = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-delete")) {
		if (elements < 3) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		if (SDDSUtil.compare(commandLineArgs[i][1],"column")) {
		    if (deleteColumns == null) {
			k = 0;
			deleteColumns = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(deleteColumns);
			deleteColumns = (String[])SDDSUtil.resize(deleteColumns, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			deleteColumns[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"parameter")) {
		    if (deleteParameters == null) {
			k = 0;
			deleteParameters = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(deleteParameters);
			deleteParameters = (String[])SDDSUtil.resize(deleteParameters, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			deleteParameters[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"array")) {
		    if (deleteArrays == null) {
			k = 0;
			deleteArrays = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(deleteArrays);
			deleteArrays = (String[])SDDSUtil.resize(deleteArrays, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			deleteArrays[k+n] = commandLineArgs[i][n+2];
		} else {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-retain")) {
		if (elements < 3) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		if (SDDSUtil.compare(commandLineArgs[i][1],"column")) {
		    if (retainColumns == null) {
			k = 0;
			retainColumns = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(retainColumns);
			retainColumns = (String[])SDDSUtil.resize(retainColumns, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			retainColumns[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"parameter")) {
		    if (retainParameters == null) {
			k = 0;
			retainParameters = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(retainParameters);
			retainParameters = (String[])SDDSUtil.resize(retainParameters, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			retainParameters[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"array")) {
		    if (retainArrays == null) {
			k = 0;
			retainArrays = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(retainArrays);
			retainArrays = (String[])SDDSUtil.resize(retainArrays, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			retainArrays[k+n] = commandLineArgs[i][n+2];
		} else {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-rename")) {
		if ((elements < 4) || ((elements % 2) != 0 /* check for even number */)) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		if (SDDSUtil.compare(commandLineArgs[i][1],"column")) {
		    if (renameColumns == null) {
			k = 0;
			renameColumns = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(renameColumns);
			renameColumns = (String[])SDDSUtil.resize(renameColumns, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			renameColumns[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"parameter")) {
		    if (renameParameters == null) {
			k = 0;
			renameParameters = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(renameParameters);
			renameParameters = (String[])SDDSUtil.resize(renameParameters, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			renameParameters[k+n] = commandLineArgs[i][n+2];
		} else if (SDDSUtil.compare(commandLineArgs[i][1],"array")) {
		    if (renameArrays == null) {
			k = 0;
			renameArrays = new String[elements - 2];
		    }
		    else {
			k = Array.getLength(renameArrays);
			renameArrays = (String[])SDDSUtil.resize(renameArrays, k + elements - 2);
		    }
		    for (n=0;n<elements-2;n++)
			renameArrays[k+n] = commandLineArgs[i][n+2];
		} else {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-pipe")) {
		if (elements == 1)
		    pipeIn = pipeOut = true;
		else if (elements == 2) {
		    if (SDDSUtil.compare(commandLineArgs[i][1],"input"))
			pipeIn = true;
		    else if (SDDSUtil.compare(commandLineArgs[i][1],"output"))
			pipeOut = true;
		    else {
			System.err.println("invalid command line argument: " + args[i]);
			printUsage();
			return;
		    }
		} else {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
	    } else {
		if ((elements != 1) || (commandLineArgs[i][0].charAt(0) == '-')) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		if (inputfile == null)
		    inputfile = commandLineArgs[i][0];
		else if (outputfile == null)
		    outputfile = commandLineArgs[i][0];
		else {
		    System.err.println("too many file names on command line");
		    printUsage();
		    return;
		}
	    }
	}
	if (pipeIn) {
	    outputfile = inputfile;
	    inputfile = null;
	}
	if (pipeOut) {
	    outputfile = null;
	}
	if ((inputfile == null) && (!pipeIn)) {
	    System.err.println("missing input file name");
	    printUsage();
	    return;
	}
	if ((outputfile == null) && (!pipeOut)) {
	    outputfile = inputfile;
	    if (!nowarnings)
		System.err.println("warning: existing file " + inputfile + " will be replaced");
	}

	SDDSFile sdds = new SDDSFile(inputfile);

	if (changeEndianIn)
	    sdds.setEndian(littleEndianIn);

	if (!sdds.readFile()) {
	    sdds.printErrors();
	    return;
        }
	
	sdds.setFileName(outputfile);
	if (changeMode)
	    sdds.setMode(binary);
	if (changeEndian)
	    sdds.setEndian(littleEndian);
	if (changeDescription)
	    sdds.setDescription(descriptionText,descriptionContents);
	if (!specifyToPage)
	    toPage = sdds.pageCount();
	if (retainParameters != null) {
	    parameterNames = sdds.getParameterNames();
	    if (parameterNames != null) {
		for (i=0;i<Array.getLength(parameterNames);i++) {
		    found = false;
		    for (j=0;j<Array.getLength(retainParameters);j++) {
			if (parameterNames[i].equals(retainParameters[j])) {
			    found = true;
			    break;
			}
		    }
		    if (!found)
			sdds.deleteParameter(parameterNames[i]);
		}
	    }
	} else if (deleteParameters != null) {
	    i = Array.getLength(deleteParameters);
	    for (j=0;j<i;j++)
		sdds.deleteParameter(deleteParameters[j]);
	}
	if (retainArrays != null) {
	    arrayNames = sdds.getArrayNames();
	    if (arrayNames != null) {
		for (i=0;i<Array.getLength(arrayNames);i++) {
		    found = false;
		    for (j=0;j<Array.getLength(retainArrays);j++) {
			if (arrayNames[i].equals(retainArrays[j])) {
			    found = true;
			    break;
			}
		    }
		    if (!found)
			sdds.deleteArray(arrayNames[i]);
		}
	    }
	} else if (deleteArrays != null) {
	    i = Array.getLength(deleteArrays);
	    for (j=0;j<i;j++)
		sdds.deleteArray(deleteArrays[j]);
	}
	if (retainColumns != null) {
	    columnNames = sdds.getColumnNames();
	    if (columnNames != null) {
		for (i=0;i<Array.getLength(columnNames);i++) {
		    found = false;
		    for (j=0;j<Array.getLength(retainColumns);j++) {
			if (columnNames[i].equals(retainColumns[j])) {
			    found = true;
			    break;
			}
		    }
		    if (!found)
			sdds.deleteColumn(columnNames[i]);
		}
	    }
	} else if (deleteColumns != null) {
	    i = Array.getLength(deleteColumns);
	    for (j=0;j<i;j++)
		sdds.deleteColumn(deleteColumns[j]);
	}
	if (renameParameters != null) {
	    for (i=0;i<Array.getLength(renameParameters) - 1;i+=2)
		sdds.renameParameter(renameParameters[i],renameParameters[i+1]);
	}
	if (renameArrays != null) {
	    for (i=0;i<Array.getLength(renameArrays) - 1;i+=2)
		sdds.renameArray(renameArrays[i],renameArrays[i+1]);
	}
	if (renameColumns != null) {
	    for (i=0;i<Array.getLength(renameColumns) - 1;i+=2)
		sdds.renameColumn(renameColumns[i],renameColumns[i+1]);
	}

	if (!sdds.openOutputFile()) {
	    sdds.printErrors();
	    return;
	}
        if (!sdds.writeLayout()) {
	    sdds.printErrors();
	    sdds.closeOutput();
	    return;
	}
	if (!sdds.writePages(fromPage,toPage)) {
	    sdds.printErrors();
	    sdds.closeOutput();
	    return;
	}
        if (!sdds.closeOutput()) {
	    sdds.printErrors();
	    return;
	}
    }
}
