/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDS3Dplot;

import javax.swing.UIManager;
import java.awt.*;
import java.lang.reflect.*;
import SDDS.java.SDDS.*;

public class Application1 {
    boolean packFrame = false;

    /**Construct the application*/
    public Application1(String inputfile) {
        Frame1 frame = new Frame1();
        //Validate frames that have preset sizes
        //Pack frames that have useful preferred size info, e.g. from their layout
        if (packFrame) {
            frame.pack();
        }
        else {
            frame.validate();
        }
        //Center the window
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension frameSize = frame.getSize();
        if (frameSize.height > screenSize.height) {
            frameSize.height = screenSize.height;
        }
        if (frameSize.width > screenSize.width) {
            frameSize.width = screenSize.width;
        }
        frame.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
        frame.setVisible(true);
        frame.setTitle("SDDS 3D Plotter");
	try {
	    frame.readFile(inputfile);
	}
	catch(Exception e) {
	    e.printStackTrace();
	}
    }
    static void printUsage() {
	String usage;
	usage = "\nusage: jsdds3dplot [-usage] [-pipe[={'input'}]] [<inputfile>]\n\n";
	usage = usage + "Program by Robert Soliday.";
	System.err.println(usage);
    }
    /**Main method*/
    public static void main(String[] args) {
	String[][] commandLineArgs;
	int arguments, elements, i;
	String inputfile=null;
	boolean pipeIn = false;
        try {
	    commandLineArgs = SDDSUtil.parseCommandLine(args);
	    arguments = Array.getLength(commandLineArgs);
	    for (i=0;i<arguments;i++) {
		elements = Array.getLength(commandLineArgs[i]);
		if (elements == 0)
		    continue;
		if (SDDSUtil.compare(commandLineArgs[i][0],"-usage")) {
		    printUsage();
		    return;
		} else if (SDDSUtil.compare(commandLineArgs[i][0],"-pipe")) {
		    if (elements == 1)
			pipeIn = true;
		    else if (elements == 2) {
			if (SDDSUtil.compare(commandLineArgs[i][1],"input"))
			    pipeIn = true;
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
		    else {
			System.err.println("too many file names on command line");
			printUsage();
			return;
		    }
		}
	    }
	    if ((inputfile != null) && (pipeIn)) {
		System.err.println("invalid command line argument: " + args[i]);
		printUsage();
		return;
	    }
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        }
        catch(Exception e) {
            e.printStackTrace();
        }
        new Application1(inputfile);
    }
}
