/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

//package SDDS.apps;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.lang.reflect.*;
import SDDS.java.SDDS.*;
import SDDS.java.SDDSplot.*;
/**
 * This program is used to plot SDDS files.<br><br>
 * usage: java SDDS.apps.sddsplot [&lt;source-file&gt;]<br>
 * [-columnnames=xColumn,yColumn[,yColumn ...]<br><br>
 *
 * @author    Robert Soliday 
 *            <a href=mailto:soliday@aps.anl.gov>
 *            soliday@aps.anl.gov</a>
 * @version   1.0 (May 9, 2000)
 * @since     JDK 1.2
 */

public class sddsplot {
    static void printUsage() {
	String usage;
	usage = "usage: java SDDS.apps.sddsplot [<source-file>]\n";
	usage = usage + "[-columnnames=xColumn,yColumn[,yColumn ...]\n\n";
	usage = usage + "This program is used to plot SDDS files.\n\n";
	usage = usage + "Program by Robert Soliday.";
	System.err.println(usage);
    }
    
    public static void main(String args[]) {
	String xColumn=null;
	String[] yColumns=null;
	String inputfile = null;
	
	double[] xData;
	double[][] yData;
	
	int rows = 0;
	int ycount = 1;
	TickInterval xticks;
	TickInterval yticks;
	String[][] commandLineArgs;
	int arguments, elements, i, j, k;

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
	    if (SDDSUtil.compare(commandLineArgs[i][0],"-columnnames")) {
		if (elements < 3) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		xColumn = commandLineArgs[i][1];
		ycount = elements - 2;
		yColumns = new String[ycount];
		for (j=2;j<elements;j++) {
		    yColumns[j-2] = commandLineArgs[i][j];
		}
	    } else {
		if (inputfile == null) {
		    inputfile = commandLineArgs[i][0];
		} else {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
	    }
	}
	if (xColumn == null) {
	    System.err.println("missing input file name");
	    printUsage();
	    return;
	}
	if (inputfile == null) {
	    System.err.println("missing xColumn name");
	    printUsage();
	    return;
	}
        JFrame frame = new JFrame("Java SDDS Plot");
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        });
        plotApplet applet = new plotApplet();
        frame.getContentPane().add("Center", applet);
        applet.init();
	applet.setColumnNames(xColumn, yColumns);
	if (applet.readData(inputfile, false, null) == false) {
	    return;
	}

        frame.pack();
        frame.setSize(new Dimension(700,500));
        frame.show();
    }
}
