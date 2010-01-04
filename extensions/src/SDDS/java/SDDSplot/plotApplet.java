package SDDS.java.SDDSplot;

import java.applet.*;
import java.awt.*;
import java.awt.geom.*;
import javax.swing.*;
import java.lang.reflect.*;
import java.net.*;
import java.util.*;
import SDDS.java.SDDS.*;


public class plotApplet extends Applet {
    final static Color bg = Color.black;
    final static Color fg = Color.white;

    static String xColumn;
    static String[] yColumns=null;

    static double[] xData;
    static double[][] yData;

    static double leftOffset = .143;
    static double rightOffset = .15;
    static double topOffset = .07;
    static double bottomOffset = .2;
    static double xOffset = .05;
    static double yOffset = .05;
    static double xmin, xmax, ymin, ymax;
    static double xlower, xupper, ylower, yupper;
    static int rows = 0;
    static int ycount = 0;
    static TickInterval xticks;
    static TickInterval yticks;

    public void init() {
	String inputfile = null;
	String ColumnNames = null;
	String endian = null;
        setBackground(bg);
        setForeground(fg);
	try {
	    if ((ColumnNames = getParameter("columns")) != null) {
		StringTokenizer st;
		st = new StringTokenizer(ColumnNames, ",");
		int length = st.countTokens();
		if (length < 2)
		    return;
		xColumn = st.nextToken();
		ycount = length - 1;
		yColumns = new String[ycount];
		int i;
		for (i=0;i<length-1;i++) {
		    yColumns[i] = st.nextToken();
		}
	    }
	} catch(Exception e) {
	};
	try {
	    endian = getParameter("endian");
	} catch(Exception e) {
	};
	try {
	    if ((inputfile = getParameter("file")) != null) {
		if (readData(inputfile, true, endian) == false) {
		}
	    }
	} catch(Exception e) {
	};
    }

    public boolean readData(String inputfile, boolean url, String endian) {
	int i, j, k;
	SDDSFile sdds = null;
	if (url) {
	    try {
		sdds = new SDDSFile(new URL(getDocumentBase(), inputfile));
	    } catch(Exception e) {
	    };
	} else {
	    sdds = new SDDSFile(inputfile);
	}
	if (endian != null) {
	    if (endian.equals("big")) {
		sdds.setEndian(false);
	    } else if (endian.equals("little")) {
		sdds.setEndian(true);
	    }
	}
	if (!sdds.readFile()) {
	    sdds.printErrors();
	    return false;
        }
	int page = 1;
	rows = sdds.getRowCount(page);
	if (rows < 1) {
	    System.err.println("input file missing column rows");
	    return false;	    
	}
	yData = new double[ycount][];
	Object[] ObjectData = new Object[rows];

	String[] columnNames;
	columnNames = sdds.getColumnNames();
	if (columnNames == null) {
	    System.err.println("input file missing specified columns");
	    return false ;
	}
	j = Array.getLength(columnNames);
	int type;
	boolean found;
	found = false;
	for (i=0;i<j;i++) {
	    if (xColumn.equals(columnNames[i])) {
		type = sdds.getColumnType(i);
		if ((type == SDDSUtil.SDDS_STRING) || (type == SDDSUtil.SDDS_CHARACTER)) {
		    System.err.println("can not plot string or character columns");
		    return false;
		}
		ObjectData = sdds.getColumnValues(i,page,false);
		xData = SDDSUtil.castArrayAsDouble(ObjectData, type);
		found = true;
		break;
	    }
	}
	if (!found) {
	    System.err.println("input file missing specified columns");
	    return false;	    
	}
	for (k=0;k<ycount;k++) {
	    found = false;
	    for (i=0;i<j;i++) {
		if (yColumns[k].equals(columnNames[i])) {
		    type = sdds.getColumnType(i);
		    if ((type == SDDSUtil.SDDS_STRING) || (type == SDDSUtil.SDDS_CHARACTER)) {
			System.err.println("can not plot string or character columns");
			return false;
		    }
		    ObjectData = sdds.getColumnValues(i,page,false);
		    yData[k] = SDDSUtil.castArrayAsDouble(ObjectData, type);
		    found = true;
		    break;
		}
	    }
	    if (!found) {
		System.err.println("input file missing specified columns");
		return false;	    
	    }
	}
	setTickMarks();
	return true;
    }

    public void setColumnNames(String xColumnName, String[] yColumnNames) {
	xColumn = xColumnName;
	yColumns = yColumnNames;
	ycount = Array.getLength(yColumns);
    }

    public void setColumnData(double[] xColumnData, double[][] yColumnData) {
	xData = xColumnData;
	yData = yColumnData;
	rows = Array.getLength(xData);
	setTickMarks();
    }

    public void setTickMarks() {
	int i, k;
	xmin = xmax = xData[0];
	ymin = ymax = yData[0][0];
	for (i=1;i<rows;i++) {
	    if (xData[i] < xmin)
		xmin = xData[i];
	    if (xData[i] > xmax)
		xmax = xData[i];
	}
	for (k=0;k<ycount;k++) {
	    for (i=1;i<rows;i++) {
		if (yData[k][i] < ymin)
		    ymin = yData[k][i];
		if (yData[k][i] > ymax)
		    ymax = yData[k][i];
	    }
	}
	double innerSpace, lower, upper;
	innerSpace = ((xmax - xmin)*10/9 - (xmax - xmin)) * 0.5;
	xlower = xmin - innerSpace;
	xupper = xmax + innerSpace;
	xticks = new TickInterval(xlower, xupper);
	innerSpace = ((ymax - ymin)*10/9 - (ymax - ymin)) * 0.5;
	ylower = ymin - innerSpace;
	yupper = ymax + innerSpace;
	yticks = new TickInterval(ylower, yupper);
    }

    int pickFont(Graphics2D g2,
		  String text,
		  int hspace,
		  int vspace,
		  String name,
		  int style,
		  int size) {
	boolean fontFits = false;
	g2.setFont(new Font(name, style, size));
        Font font = g2.getFont();
        FontMetrics fontMetrics = g2.getFontMetrics();

	while (!fontFits) {
	    if ((fontMetrics.stringWidth(text) <= hspace) &&
		(fontMetrics.getHeight() <= vspace)) {
		fontFits = true;
	    } else if ( size > 6 ) {
		g2.setFont(font = new Font(name,
					   style,
					   --size));
		fontMetrics = g2.getFontMetrics();
	    } else {
		fontFits = true;
	    }
	}
        return fontMetrics.stringWidth(text);
    }

    public void paint(Graphics g) {
	
        Graphics2D g2 = (Graphics2D) g;
	AffineTransform aft = new AffineTransform();
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        Dimension d = getSize();

	int tOffset = (int)(d.height * topOffset);
	int bOffset = (int)(d.height * bottomOffset);
	int lOffset = (int)(d.width * leftOffset);
	int rOffset = (int)(d.width * rightOffset);
        int rectWidth = d.width - lOffset - rOffset;
        int rectHeight = d.height - tOffset - bOffset;
	int innerLeftOffset = (int)(lOffset + (xOffset * rectWidth));
	int innerTopOffset = (int)(tOffset + (yOffset * rectHeight));
        int innerRectWidth = (int)(rectWidth - 2*xOffset*rectWidth);
        int innerRectHeight = (int)(rectHeight - 2*yOffset*rectHeight);

	double xincr = innerRectWidth / (xmax - xmin);
	double yincr = innerRectHeight / (ymax - ymin);

	int length, i, j, yFontHeight;
	String label;

        g2.draw(new Rectangle2D.Double(lOffset, tOffset, rectWidth, rectHeight));
	double pos;
	for (i = 0; i < xticks.number; i++) {
	    pos = (xticks.start + (xticks.delta * i) - xmin)*xincr+innerLeftOffset;
	    g2.draw(new Line2D.Double(pos, tOffset, pos, tOffset + (yOffset * rectHeight * 0.5)));
	    g2.draw(new Line2D.Double(pos, tOffset+rectHeight, pos, tOffset + rectHeight - (yOffset * rectHeight * 0.5)));
	    label = Double.toString(xticks.start + (xticks.delta * i));
	    length = pickFont(g2, label, (int)(xticks.delta * xincr * .9), (int)(bOffset * .3), "Courier", Font.PLAIN, 25);
	    g2.drawString(label, (int)(pos - (length * .5)), (int)(tOffset + rectHeight + (bOffset * .25)));
	}
	for (i = 0; i < yticks.number; i++) {
	    pos = (ymax - (yticks.start + (yticks.delta * i)))*yincr+innerTopOffset;
	    g2.draw(new Line2D.Double(lOffset, pos, lOffset + (xOffset * rectWidth * 0.25), pos));
	    g2.draw(new Line2D.Double(lOffset+rectWidth, pos, lOffset + rectWidth - (xOffset * rectWidth * 0.25), pos));
	    label = Double.toString(yticks.start + (yticks.delta * i));
	    length = pickFont(g2, label, (int)(lOffset * .4), (int)(yticks.delta * yincr * .9), "Courier", Font.PLAIN, 25);
	    g2.drawString(label, (int)(lOffset * .6), (int)(pos));
	    
	}

	length = pickFont(g2, xColumn, rectWidth, (int)(bOffset * .4), "Courier", Font.PLAIN, 25);
	g2.drawString(xColumn, (int)(lOffset + ((rectWidth - length) * .5)), (int)(tOffset + rectHeight + (bOffset * .65)));

	label = yColumns[0];
	for (j = 1; j < ycount; j++) {
	    label = label.concat(", " + yColumns[j]);
	}
	yFontHeight = (int)((lOffset * .4));
	length = pickFont(g2, label, rectHeight, yFontHeight, "Courier", Font.PLAIN, 25);
	aft.setToTranslation(yFontHeight, ((rectHeight + length) * 0.5) + tOffset);
	g2.transform(aft);
	aft.setToRotation(Math.PI * 1.5);
	g2.transform(aft);
	g2.drawString(label, 0, 0);
	aft.setToRotation(Math.PI * 0.5);
	g2.transform(aft);
	aft.setToTranslation(-yFontHeight, -((rectHeight + length) * 0.5) - tOffset);
	g2.transform(aft);
	for (j = 0; j < ycount; j++) {
	    for (i = 0; i < rows - 1; i++) {		
		g2.draw(new Line2D.Double((xData[i]-xmin)*xincr+innerLeftOffset, 
					  (ymax-yData[j][i])*yincr+innerTopOffset, 
					  (xData[i+1]-xmin)*xincr+innerLeftOffset, 
					  (ymax-yData[j][i+1])*yincr+innerTopOffset));
		
	    }
	}
    }
}

