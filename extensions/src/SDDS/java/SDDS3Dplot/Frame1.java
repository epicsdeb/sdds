/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution.
\*************************************************************************/

package SDDS.java.SDDS3Dplot;

import java.io.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import java.lang.reflect.*;
import visad.*;
import visad.util.*;
import visad.java3d.DisplayImplJ3D;
import visad.java3d.ProjectionControlJ3D;
import java.rmi.RemoteException;
import SDDS.java.SDDS.*;
import SDDS.java.SDDS.SDDSUtil;
import javax.swing.event.*;
import com.sun.image.codec.jpeg.*;

public class Frame1 extends JFrame {
    protected FileDialog fileDialog;
    JPanel contentPane;
    JMenuBar jMenuBar1 = new JMenuBar();
    JMenu jMenuFile = new JMenu();
    JMenuItem jMenuFileExit = new JMenuItem();
    BorderLayout borderLayout1 = new BorderLayout();

    static DisplayImpl display;
   // AnimationWidget animWid;
    static ScalarMap xMap, yMap, zMap, zColorMap, selValMap, xRangeMap, yRangeMap, zRangeMap;
    SDDSFile sdds;
    int contourType;
    boolean firstPlot;
    static int colorCount = 6;
    boolean scatterPlot = false;

    JMenuItem jMenuItem1 = new JMenuItem();
    JMenu jMenu1 = new JMenu();
    JMenu jMenu2 = new JMenu();
    JMenuItem jMenuItem2 = new JMenuItem();
    JMenuItem jMenuItem3 = new JMenuItem();
    JMenuItem jMenuItem4 = new JMenuItem();
    JPanel jPanel1 = new JPanel();
    JLabel jLabel1 = new JLabel();
    GridBagLayout gridBagLayout1 = new GridBagLayout();
    JLabel jLabel2 = new JLabel();
    JLabel jLabel3 = new JLabel();
    JTextField jTextField1 = new JTextField();
    JTextField jTextField2 = new JTextField();
    JLabel jLabel4 = new JLabel();
    JPanel jPanel2 = new JPanel();
    JComboBox jComboBox1 = new JComboBox();
    JComboBox jComboBox2 = new JComboBox();
    JButton jButton1 = new JButton();
    JLabel jLabel5 = new JLabel();
    JComboBox jComboBox3 = new JComboBox();
    JMenuItem jMenuItem5 = new JMenuItem();
    JMenuItem jMenuItem7 = new JMenuItem();
    JComboBox jComboBox4 = new JComboBox();
    JComboBox jComboBox5 = new JComboBox();
    JMenuItem jMenuItem10 = new JMenuItem();
  private GridBagLayout gridBagLayout2 = new GridBagLayout();
  private JCheckBox jCheckBox1 = new JCheckBox();

    /**Construct the frame*/
    public Frame1() {
        enableEvents(AWTEvent.WINDOW_EVENT_MASK);
        try {
            jbInit();
        }
        catch(Exception e) {
            e.printStackTrace();
        }
    }
    /**Component initialization*/
    private void jbInit() throws Exception,RemoteException  {
        //setIconImage(Toolkit.getDefaultToolkit().createImage(Frame1.class.getResource("[Your Icon]")));
        contentPane = (JPanel) this.getContentPane();
        contentPane.setLayout(borderLayout1);
        this.setSize(new Dimension(400, 520));
        this.setTitle("Frame Title");
        JPopupMenu.setDefaultLightWeightPopupEnabled(false);
        jMenuFile.setText("File");
        jMenuFileExit.setText("Exit");
        jMenuFileExit.addActionListener(new ActionListener()  {
            public void actionPerformed(ActionEvent e) {
                jMenuFileExit_actionPerformed(e);
            }
        });
        jMenuItem1.setText("Open");
        jMenuItem1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem1_actionPerformed(e);
            }
        });
        jMenu1.setText("Options");
        jMenu2.setText("Background");
        jMenuItem2.setText("White");
        jMenuItem2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem2_actionPerformed(e);
            }
        });
        jMenuItem3.setText("Black");
        jMenuItem3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem3_actionPerformed(e);
            }
        });
        jMenuItem4.setText("Grey");
        jMenuItem4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem4_actionPerformed(e);
            }
        });
        jLabel1.setText("X");
        jPanel1.setLayout(gridBagLayout1);
        jLabel2.setText("Y");
        jLabel3.setText("Z");
        jLabel4.setText("Color");
        jTextField1.setMargin(new Insets(0, 3, 0, 0));
        jTextField1.setColumns(10);
        jTextField2.setMinimumSize(new Dimension(7, 21));
        jTextField2.setMargin(new Insets(0, 3, 0, 0));
        jTextField2.setColumns(10);
        jButton1.setText("Plot");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton1_actionPerformed(e);
            }
        });
        jLabel5.setText("Page");
        jMenuItem5.setText("Save JPEG");
        jMenuItem5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem5_actionPerformed(e);
            }
        });
        jMenuItem7.setText("Color");
        jMenuItem7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem7_actionPerformed(e);
            }
        });
        jMenuItem10.setText("Controls");
        jMenuItem10.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jMenuItem10_actionPerformed(e);
            }
        });
    jPanel2.setLayout(gridBagLayout2);
    jCheckBox1.setText("Force 3 column plot");
    jCheckBox1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(ActionEvent e) {
        jCheckBox1_actionPerformed(e);
      }
    });
    jMenuFile.add(jMenuItem1);
        jMenuFile.add(jMenuItem5);
        jMenuFile.add(jMenuFileExit);
        jMenuBar1.add(jMenuFile);
        jMenuBar1.add(jMenu1);
        display = new DisplayImplJ3D("display1");
        GraphicsModeControl dispGMC = (GraphicsModeControl)  display.getGraphicsModeControl();
        dispGMC.setScaleEnable(true);
        dispGMC.setTextureEnable(false);

        contentPane.add(display.getComponent(), BorderLayout.CENTER);

        jMenu1.add(jMenu2);
        jMenu1.add(jMenuItem7);
        jMenu1.add(jMenuItem10);
        jMenu2.add(jMenuItem2);
        jMenu2.add(jMenuItem3);
        jMenu2.add(jMenuItem4);
        contentPane.add(jPanel1, BorderLayout.SOUTH);
        jPanel1.add(jTextField1, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.SOUTH, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jTextField2, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jLabel4, new GridBagConstraints(0, 3, 1, 1, 0.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 10, 0, 10), 0, 0));
        jPanel1.add(jLabel2, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 10, 0, 10), 0, 0));
        jPanel1.add(jLabel3, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 10, 0, 10), 0, 0));
        jPanel1.add(jLabel1, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(10, 10, 0, 10), 0, 0));
        jPanel1.add(jPanel2, new GridBagConstraints(2, 1, 3, 1, 1.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
    jPanel2.add(jCheckBox1,      new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jComboBox1, new GridBagConstraints(1, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jComboBox2, new GridBagConstraints(1, 3, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jButton1, new GridBagConstraints(3, 2, 1, 2, 0.0, 0.0
            ,GridBagConstraints.NORTHWEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jLabel5, new GridBagConstraints(2, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.SOUTH, GridBagConstraints.NONE, new Insets(0, 20, 0, 10), 0, 0));
        jPanel1.add(jComboBox3, new GridBagConstraints(3, 0, 2, 1, 0.0, 0.0
            ,GridBagConstraints.SOUTHWEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jComboBox4, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.SOUTH, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(jComboBox5, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.SOUTH, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 0), 0, 0));
        jComboBox4.setVisible(false);
        jComboBox5.setVisible(false);
        jCheckBox1.setVisible(false);
        this.setJMenuBar(jMenuBar1);
        firstPlot = true;
    }
    /**File | Exit action performed*/
    public void jMenuFileExit_actionPerformed(ActionEvent e) {
        System.exit(0);
    }
    /**Overridden so we can exit when window is closed*/
    protected void processWindowEvent(WindowEvent e) {
        super.processWindowEvent(e);
        if (e.getID() == WindowEvent.WINDOW_CLOSING) {
            jMenuFileExit_actionPerformed(null);
        }
    }
    public void readFile(String fileName) throws RemoteException, VisADException {
        sdds = new SDDSFile(fileName);
	if (!sdds.readFile()) {
	    sdds.printErrors();
	    return;
        }
	loadFile();
    }

    public void loadFile() throws RemoteException, VisADException {
        int index, pages;

        contourType = 1;
	index = sdds.getParameterIndex("Variable1Name");
	if ((index == -1) || (jCheckBox1.isSelected() == true)) {
            loadFile2();
	    return;
	}
        jComboBox4.setVisible(false);
        jTextField1.setVisible(true);
        jComboBox5.setVisible(false);
        jTextField2.setVisible(true);
        jCheckBox1.setVisible(true);

        jTextField1.setText((String)sdds.getParameterValue(index,1,false));

	index = sdds.getParameterIndex("Variable2Name");
	if (index == -1) {
	    return;
	}
        jTextField2.setText((String)sdds.getParameterValue(index,1,false));

        jComboBox1.removeAllItems();
        jComboBox2.removeAllItems();
        jComboBox3.removeAllItems();
        String[] columnNames = sdds.getColumnNames();
        for (int i = 0; i < Array.getLength(columnNames); i++) {
            jComboBox1.addItem(columnNames[i]);
            jComboBox2.addItem(columnNames[i]);
        }
        pages = sdds.pageCount();
        for (int i = 1; i <= pages; i++) {
            jComboBox3.addItem(String.valueOf(i));
        }
    }
    public void loadFile2() throws RemoteException, VisADException {
        int pages, i, j, length, maxhits, hits;
        Double test;
        String colPrefix = new String();
        contourType = 2;
        jComboBox1.removeAllItems();
        jComboBox2.removeAllItems();
        jComboBox3.removeAllItems();
        jComboBox4.removeAllItems();
        jComboBox5.removeAllItems();
        String[] columnNames = sdds.getColumnNames();
        length = Array.getLength(columnNames);
        String[] columnNamesWithoutNumbers = new String[length];
        boolean[] checked = new boolean[length];
        for (i = 0; i < length; i++) {
            columnNamesWithoutNumbers[i] = columnNames[i];
            for (j = columnNames[i].length() - 1; j > 0; j--) {
                try {
                    test = new Double(columnNames[i].substring(j));
                } catch (NumberFormatException ex) {
                    columnNamesWithoutNumbers[i] = columnNames[i].substring(0,j+1);
                    break;
                }
            }
        }
        i = 0;
        maxhits = 0;
        while (i < length) {
            if (checked[i]) {
                i++;
                continue;
            }
            checked[i] = true;
            hits = 0;
            for (j = i+1; j < length; j++) {
                if (columnNamesWithoutNumbers[i].equals(columnNamesWithoutNumbers[j])) {
                    checked[j] = true;
                    hits++;
                    if (hits > maxhits) {
                        maxhits = hits;
                        if (colPrefix.equals(columnNamesWithoutNumbers[i]) == false) {
                            colPrefix = columnNamesWithoutNumbers[i];
                        }
                    }
                }
            }
            i++;
        }

        jTextField1.setVisible(false);
        jComboBox4.setVisible(true);
        jTextField1.setText("");
        jTextField2.setText("");
        if ((maxhits < 2) || (jCheckBox1.isSelected() == true)) {
            contourType = 3;
            jTextField2.setVisible(false);
            jComboBox5.setVisible(true);
            jCheckBox1.setVisible(true);
            for (i = 0; i < length; i++) {
              /*  if (colPrefix.equals(columnNamesWithoutNumbers[i]) == false) {*/
                    j = sdds.getColumnType(i);
                    if ((j != SDDSUtil.SDDS_CHARACTER) && (j != SDDSUtil.SDDS_STRING)) {
                        jComboBox1.addItem(columnNames[i]);
                        jComboBox2.addItem(columnNames[i]);
                        jComboBox4.addItem(columnNames[i]);
                        jComboBox5.addItem(columnNames[i]);
                    }
             /*   }*/
            }
            if (jComboBox1.getItemCount() >= 3) {
                jComboBox4.setSelectedIndex(0);
                jComboBox5.setSelectedIndex(1);
                jComboBox1.setSelectedIndex(2);
                jComboBox2.setSelectedIndex(2);
            }
        } else {
            jTextField2.setVisible(true);
            jComboBox5.setVisible(false);
            jCheckBox1.setVisible(true);
            for (i = 0; i < length; i++) {
                if (colPrefix.equals(columnNamesWithoutNumbers[i]) == false) {
                    j = sdds.getColumnType(i);
                    if ((j != SDDSUtil.SDDS_CHARACTER) && (j != SDDSUtil.SDDS_STRING)) {
                        jComboBox4.addItem(columnNames[i]);
                    }
                }
            }
            jComboBox1.addItem(colPrefix);
            jComboBox2.addItem(colPrefix);
        }
        pages = sdds.pageCount();
        for (i = 1; i <= pages; i++) {
            jComboBox3.addItem(String.valueOf(i));
        }
    }
    public void plotFile() throws RemoteException, VisADException {
        RealType x, y, z, zColor, frame;
        RealTupleType domain_tuple, range_tuple;
        FunctionType func_domain_range, func_domain_range_frame;
        Set domain_set;
        Set frame_set;
        FlatField vals_ff;
        FieldImpl frameField;
        DataReferenceImpl data_ref;
        boolean fourDimensional = false;

        int index, index2, pages, page;

        if (sdds == null)
            return;

        float[][] myColorTable;
        if (firstPlot) {
	    myColorTable = new float[][]{{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},  // red
					           {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},  // green
					           {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; // blue
        } else {
            ColorControl colCont = (ColorControl) zColorMap.getControl();
            myColorTable = new float[3][colorCount];
            float[][] colorTable = colCont.getTable();
            for (int i=0; i<colorCount; i++) {
                myColorTable[0][i] = colorTable[0][i*100];
                myColorTable[1][i] = colorTable[1][i*100];
                myColorTable[2][i] = colorTable[2][i*100];
            }
        }


        frame = RealType.getRealType("Frame");

        if (contourType == 1) {
            x = RealType.getRealType(jTextField1.getText(),SI.meter,null);
	    y = RealType.getRealType(jTextField2.getText(),SI.meter,null);
            z = RealType.ZAxis;
        } else if (contourType == 2) {
            x = RealType.getRealType((String)jComboBox4.getSelectedItem(),SI.meter,null);
	    y = RealType.getRealType(jTextField2.getText(),SI.meter,null);
            z = RealType.ZAxis;
        } else {
            x = RealType.getRealType((String)jComboBox4.getSelectedItem(),SI.meter,null);
	    y = RealType.getRealType((String)jComboBox5.getSelectedItem(),SI.meter,null);
	    z = RealType.getRealType((String)jComboBox1.getSelectedItem(),SI.meter,null);
        }

        domain_tuple = new RealTupleType(x,y);
        // pages = sdds.pageCount();
        page = (Integer.valueOf((String)jComboBox3.getSelectedItem())).intValue();
        pages = 1;
        Object[][] zValues = new Object[pages][];
        int colFound = 0;
        Object[] colValues = null;
        double colRange[] = new double[2];
        double[] colYRange = null;
        Object[][] colorValues = new Object[pages][];
        if (contourType == 1) {
            if (jComboBox1.getSelectedIndex() != jComboBox2.getSelectedIndex()) {
                fourDimensional = true;
            }
	    index = sdds.getColumnIndex((String)jComboBox1.getSelectedItem());
            for (int i = 0; i < pages; i++) {
	        zValues[i] = sdds.getColumnValues(index,page,false);
            }
            if (fourDimensional) {
	        index = sdds.getColumnIndex((String)jComboBox2.getSelectedItem());
                for (int i = 0; i < pages; i++) {
	            colorValues[i] = sdds.getColumnValues(index,page,false);
                }
            }
        } else if (contourType == 2) {
            String[] columnNames = sdds.getColumnNames();
            int colCount = Array.getLength(columnNames);
            boolean[] columnFound = new boolean[colCount];
            String zName = (String)jComboBox1.getSelectedItem();
            for (int j=0; j<colCount; j++) {
                if (SDDSUtil.compare(zName,(String)columnNames[j])) {
                    columnFound[j] = true;
                    colFound++;
                }
            }
            colYRange = new double[colFound];
            colFound = 0;
            for (int j=0; j<colCount; j++) {
                if (columnFound[j]) {
                    colYRange[colFound] = new Double((String)columnNames[j].substring(zName.length())).doubleValue();
                    colFound++;
                }
            }
            colRange[0] = colYRange[0];
            colRange[1] = colYRange[colFound-1];
            int rowCount;
            int found = 0;
            for (int i = 0; i < pages; i++) {
                rowCount = sdds.getRowCount(i+1);
                zValues[i] = new Object[rowCount * colFound];
                for (int j=0; j<colCount; j++) {
                    if (columnFound[j]) {
                        colValues = sdds.getColumnValues(j,page,false);
                        for (int k=0; k<rowCount; k++) {
                            zValues[i][k * colFound + found] = colValues[k];
                        }
                        found++;
                    }
                }
            }
        } else {
            if (jComboBox1.getSelectedIndex() != jComboBox2.getSelectedIndex()) {
                fourDimensional = true;
            }
	    index = sdds.getColumnIndex((String)jComboBox1.getSelectedItem());
            for (int i = 0; i < pages; i++) {
	        zValues[i] = sdds.getColumnValues(index,page,false);
            }
            if (fourDimensional) {
	        index = sdds.getColumnIndex((String)jComboBox2.getSelectedItem());
                for (int i = 0; i < pages; i++) {
	            colorValues[i] = sdds.getColumnValues(index,page,false);
                }
            }
        }
	zColor = RealType.getRealType("zColorRealType",SI.meter,null);
        range_tuple = new RealTupleType( z, zColor  );
        func_domain_range = new FunctionType( domain_tuple, range_tuple);
        func_domain_range_frame = new FunctionType( frame, func_domain_range);

        int NROWS = 0;
        int NCOLS = 0;
        double xMinimum=0, xInterval=0, yMinimum=0, yInterval=0;
        if (contourType == 1) {
	    index2 = sdds.getParameterIndex("Variable1Name");
	    if (index2 == -1) {
	        return;
	    }
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Interval");
	    if (index == -1) {
	        return;
	    }
	    xInterval = ((Number)sdds.getParameterValue(index,page,false)).doubleValue();
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Minimum");
	    if (index == -1) {
	        return;
	    }
	    xMinimum = ((Number)sdds.getParameterValue(index,page,false)).doubleValue();
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Dimension");
	    if (index == -1) {
	        return;
	    }
	    NROWS = ((Number)sdds.getParameterValue(index,page,false)).intValue();

	    index2 = sdds.getParameterIndex("Variable2Name");
	    if (index2 == -1) {
	        return;
	    }
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Interval");
	    if (index == -1) {
	        return;
	    }
	    yInterval = ((Number)sdds.getParameterValue(index,page,false)).doubleValue();
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Minimum");
	    if (index == -1) {
	        return;
	    }
	    yMinimum = ((Number)sdds.getParameterValue(index,page,false)).doubleValue();
	    index = sdds.getParameterIndex((String)sdds.getParameterValue(index2,1,false) + "Dimension");
	    if (index == -1) {
	        return;
	    }
            NCOLS = ((Number)sdds.getParameterValue(index,page,false)).intValue();
        } else if (contourType == 2) {
            double xMaximum;
            NROWS = sdds.getRowCount(page);
            NCOLS = colFound;
            colValues = sdds.getColumnValues(sdds.getColumnIndex((String)jComboBox4.getSelectedItem()),page,false);
            xMinimum = ((Number)colValues[0]).doubleValue();
            xInterval = (((Number)colValues[NROWS - 1]).doubleValue() - ((Number)colValues[0]).doubleValue()) / (NROWS - 1.0);
            yMinimum = colRange[0];
            yInterval = (colRange[1] - colRange[0]) / (NCOLS - 1.0);
        } else {
            NROWS = sdds.getRowCount(page);

        }

        if (contourType == 1) {
            float[][] ir2Dset = new float[2][NROWS * NCOLS];
            float ir2Dset_temp;
            for(int r = 0; r < NROWS; r++) {
                ir2Dset_temp = (float)(xMinimum + (xInterval * r));
                for(int c = 0; c < NCOLS; c++) {
                    ir2Dset[0][ c * NROWS + r ] = ir2Dset_temp;
                    ir2Dset[1][ c * NROWS + r ] = (float)(yMinimum + (yInterval * c));
                }
            }
            domain_set = new Gridded2DSet(domain_tuple,ir2Dset, NROWS, NCOLS);
        } else if (contourType == 2) {
            float[][] ir2Dset = new float[2][NROWS * NCOLS];
            for(int r = 0; r < NROWS; r++) {
                for(int c = 0; c < NCOLS; c++) {
                    ir2Dset[0][ c * NROWS + r ] = ((Number)colValues[r]).floatValue();
                    ir2Dset[1][ c * NROWS + r ] = (float)(colYRange[c]);
                }
            }
            domain_set = new Gridded2DSet(domain_tuple,ir2Dset, NROWS, NCOLS);
        } else {
            float[][] ir2Dset = new float[2][NROWS];
            Object[] colYValues;
            colValues = sdds.getColumnValues(sdds.getColumnIndex((String)jComboBox4.getSelectedItem()),page,false);
            colYValues = sdds.getColumnValues(sdds.getColumnIndex((String)jComboBox5.getSelectedItem()),page,false);
            for(int r = 0; r < NROWS; r++) {
                ir2Dset[0][r] = ((Number)colValues[r]).floatValue();
                ir2Dset[1][r] = ((Number)colYValues[r]).floatValue();
            }
            domain_set = new Irregular2DSet(domain_tuple,ir2Dset);
        }

        frame_set = new Linear1DSet(frame, 0.0, 0.0 + pages, pages);

	float[][] flat_samples;
        if (contourType == 3) {
            flat_samples = new float[2][NROWS];
        } else {
            flat_samples = new float[2][NCOLS * NROWS];
        }
	vals_ff = new FlatField( func_domain_range, domain_set);
        frameField = new FieldImpl( func_domain_range_frame, frame_set);

        if (contourType == 3) {
            if (fourDimensional) {
                for (int p = 0; p < pages; p++) {
                    for(int r = 0; r < NROWS; r++) {
                        flat_samples[0][r] = ((Number)zValues[p][r]).floatValue();
                        flat_samples[1][r] = ((Number)colorValues[p][r]).floatValue();
                    }
                    vals_ff.setSamples( flat_samples);
                    frameField.setSample(p, vals_ff);
                }
            } else {
                for (int p = 0; p < pages; p++) {
                    for(int r = 0; r < NROWS; r++) {
                        flat_samples[0][r] = ((Number)zValues[p][r]).floatValue();
                        flat_samples[1][r] = flat_samples[0][r];
	            }
	            vals_ff.setSamples( flat_samples);
                    frameField.setSample(p, vals_ff);
                }
            }
        } else {
            if (fourDimensional) {
                for (int p = 0; p < pages; p++) {
                    for(int r = 0; r < NROWS; r++) {
	                for(int c = 0; c < NCOLS; c++) {
                            flat_samples[0][ c * NROWS + r ] = ((Number)zValues[p][r * NCOLS + c]).floatValue();
                            flat_samples[1][ c * NROWS + r ] = ((Number)colorValues[p][r * NCOLS + c]).floatValue();
                        }
                    }
                    vals_ff.setSamples( flat_samples);
                    frameField.setSample(p, vals_ff);
                }
            } else {
                for (int p = 0; p < pages; p++) {
                    for(int r = 0; r < NROWS; r++) {
	                for(int c = 0; c < NCOLS; c++) {
		            flat_samples[0][ c * NROWS + r ] = ((Number)zValues[p][r * NCOLS + c]).floatValue();
		            flat_samples[1][ c * NROWS + r ] = flat_samples[0][ c * NROWS + r ];
	                }
	            }
	            vals_ff.setSamples( flat_samples);
                    frameField.setSample(p, vals_ff);
                }
            }
        }

        display.removeAllReferences();
        display.clearMaps();

        if ((firstPlot) && (contourType == 3)) {
            GraphicsModeControl dispGMC = (GraphicsModeControl)  display.getGraphicsModeControl();
            dispGMC.setPointMode(true);
            dispGMC.setPointSize(2f);
        }

	xMap = new ScalarMap( x, Display.XAxis );
	yMap = new ScalarMap( y, Display.YAxis );
	zMap = new ScalarMap( z,  Display.ZAxis );
	xRangeMap = new ScalarMap( x, Display.SelectRange );
	yRangeMap = new ScalarMap( y, Display.SelectRange );
	zRangeMap = new ScalarMap( z,  Display.SelectRange );
	zColorMap = new ScalarMap( zColor,  Display.RGB );
        selValMap = new ScalarMap( frame, Display.Animation );

	display.addMap( xMap );
	display.addMap( yMap );
       	display.addMap( zMap );
	display.addMap( xRangeMap );
	display.addMap( yRangeMap );
       	display.addMap( zRangeMap );
	display.addMap( zColorMap );
        display.addMap( selValMap );

        extrapolateColors(myColorTable);

	data_ref = new DataReferenceImpl("data_ref");
	data_ref.setData( frameField );
	display.addReference( data_ref );
        firstPlot = false;
    }

    void jMenuItem1_actionPerformed(ActionEvent e) {
        try {
	    if (fileDialog == null) {
		fileDialog = new FileDialog(this);
	    }
	    fileDialog.setMode(FileDialog.LOAD);
            fileDialog.setFile("");
	    fileDialog.show();

	    String file = fileDialog.getFile();
	    if (file == null) {
		return;
	    }
	    String directory = fileDialog.getDirectory();
	    File f = new File(directory, file);
	    if (f.exists()) {
//                display.removeAllReferences();
//                display.clearMaps();
                readFile(f.getAbsolutePath());
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        }

    }

    void jMenuItem2_actionPerformed(ActionEvent e) {
        try {
            DisplayRenderer dRenderer = display.getDisplayRenderer();
            dRenderer.setBackgroundColor(1f,1f,1f);
            dRenderer.setBoxColor(0f,0f,0f);
            dRenderer.setCursorColor(0f,0f,0f);
            float[] b = colorToFloats(Color.black);
            if (xMap != null) {
                xMap.setScaleColor(b);
                yMap.setScaleColor(b);
                zMap.setScaleColor(b);
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    void jMenuItem3_actionPerformed(ActionEvent e) {
        try {
            DisplayRenderer dRenderer = display.getDisplayRenderer();
            dRenderer.setBackgroundColor(0f,0f,0f);
            dRenderer.setBoxColor(1f,1f,1f);
            dRenderer.setCursorColor(1f,1f,1f);
            float[] w = colorToFloats(Color.white);
            if (xMap != null) {
                xMap.setScaleColor(w);
                yMap.setScaleColor(w);
                zMap.setScaleColor(w);
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    void jMenuItem4_actionPerformed(ActionEvent e) {
        try {
            DisplayRenderer dRenderer = display.getDisplayRenderer();
            dRenderer.setBackgroundColor(0.7f,0.7f,0.7f);
            dRenderer.setBoxColor(0f,0f,0f);
            dRenderer.setCursorColor(0f,0f,0f);
            float[] b = colorToFloats(Color.black);
            if (xMap != null) {
                xMap.setScaleColor(b);
                yMap.setScaleColor(b);
                zMap.setScaleColor(b);
            }
        } catch(Exception ex) {
            ex.printStackTrace();
        }
    }
    /* Utility method to transform a Java color in
       an array of rgb components between 0 and 1*/
    static float[] colorToFloats(Color c) {
        float[] rgb = new float[]{0.5f,0.5f,0.5f};  //init with gray
        if (c != null) {
            rgb[0] = (float) c.getRed()/255.0f;
            rgb[1] = (float) c.getGreen()/255.0f;
            rgb[2] = (float) c.getBlue()/255.0f;
        }
        return rgb;
    }

    void jButton1_actionPerformed(ActionEvent e) {
        try {
            plotFile();
        } catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    void jMenuItem5_actionPerformed(ActionEvent e) {
       saveDisplay();
    }


    void saveDisplay(){

        Runnable captureImage = new Runnable() {
            public void run() {
                try {
	            if (fileDialog == null) {
		        fileDialog = new FileDialog(Frame1.this);
	            }
	            fileDialog.setMode(FileDialog.SAVE);
                    fileDialog.setFile("");
	            fileDialog.show();

	            String cfn = fileDialog.getFile();
	            if (cfn == null) {
		        return;
	            }
	            String dir = fileDialog.getDirectory();

                        BufferedImage image = display.getImage();

                        FileOutputStream ffout = new FileOutputStream(dir+File.separator+cfn);
                        JPEGEncodeParam jepar = JPEGCodec.getDefaultJPEGEncodeParam(image);
                        jepar.setQuality( 1.0f, true);

                        JPEGImageEncoder jpege = JPEGCodec.createJPEGEncoder(ffout) ;
                        jpege.encode(image, jepar);
                        ffout.close();
                } catch(FileNotFoundException e) {
                    e.printStackTrace();
                } catch(IOException e){
                    e.printStackTrace();
                }
            }
        };
        Thread t = new Thread(captureImage);
        t.start();
    }

    void jMenuItem7_actionPerformed(ActionEvent e) {
        Dialog1 range = new Dialog1();
        if (xMap == null)
            return;
        range.pack();
        range.setLocationRelativeTo(jPanel2);
        ColorControl colCont = (ColorControl) zColorMap.getControl();
        float[][] myColorTable = new float[3][colorCount];
        float[][] colorTable = colCont.getTable();
        for (int i=0; i<colorCount; i++) {
            myColorTable[0][i] = colorTable[0][i*100];
            myColorTable[1][i] = colorTable[1][i*100];
            myColorTable[2][i] = colorTable[2][i*100];
        }
        range.setColors(myColorTable);
        range.setVisible(true);
    }

    static void setPlotColors(float[][] myColorTable) {
        try {
            ColorControl colCont = (ColorControl) zColorMap.getControl();
            colCont.setTable(myColorTable);
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    static void extrapolateColors(float[][] myColorTable) {
        int numOfColors = Array.getLength(myColorTable[0]);
        colorCount = numOfColors;
        if (numOfColors == 1) {
            setPlotColors(myColorTable);
            return;
        }
        float[][] newColorTable = new float[3][(numOfColors-1)*100+1];
        int i, j;
        float r,g,b;
        for (i=0; i < numOfColors-1; i++) {
            r = (myColorTable[0][i+1] - myColorTable[0][i])*.01f;
            g = (myColorTable[1][i+1] - myColorTable[1][i])*.01f;
            b = (myColorTable[2][i+1] - myColorTable[2][i])*.01f;
            for (j=0; j < 100; j++) {
                newColorTable[0][i*100+j] = myColorTable[0][i] + r * j;
                newColorTable[1][i*100+j] = myColorTable[1][i] + g * j;
                newColorTable[2][i*100+j] = myColorTable[2][i] + b * j;
            }
        }
        newColorTable[0][(numOfColors-1)*100] = myColorTable[0][numOfColors-1];
        newColorTable[1][(numOfColors-1)*100] = myColorTable[1][numOfColors-1];
        newColorTable[2][(numOfColors-1)*100] = myColorTable[2][numOfColors-1];
        setPlotColors(newColorTable);
    }

    static void setOrthoView(int orientation) {
        try {
            ProjectionControlJ3D pc3d = (ProjectionControlJ3D)display.getProjectionControl();
            pc3d.setOrthoView(orientation);
        } catch(Exception ex){
            ex.printStackTrace();
        }
    }

    void jMenuItem10_actionPerformed(ActionEvent e) {
        try {
            Dialog4 controls = new Dialog4();
            controls.pack();
            controls.setLocationRelativeTo(jPanel2);

            double[] xRange, yRange, zRange, zColorRange;
            if (xMap == null) {
                xRange = new double[]{0d, 100d};
                yRange = new double[]{0d, 100d};
                zRange = new double[]{0d, 100d};
                zColorRange = new double[]{0d, 100d};
            } else {
                xRange = xMap.getRange();
                yRange = yMap.getRange();
                zRange = zMap.getRange();
                zColorRange = zColorMap.getRange();
            }
	    GraphicsModeControl dispGMC = (GraphicsModeControl)  display.getGraphicsModeControl();
            controls.getDisplayProperties(dispGMC.getScaleEnable(), dispGMC.getPointMode(),
                                          dispGMC.getTextureEnable(), dispGMC.getPointSize(),
                                          xRange, yRange, zRange, zColorRange);
            controls.setVisible(true);
        } catch(Exception ex){
            ex.printStackTrace();
        }
    }

    static void setDisplayProperties(boolean enableScale, boolean pointMode,
                                     boolean textureMapping, float pointSize,
                                     double[] ranges) {
        try {
	    GraphicsModeControl dispGMC = (GraphicsModeControl)  display.getGraphicsModeControl();
	    dispGMC.setScaleEnable(enableScale);
            dispGMC.setPointMode(pointMode);
    	    dispGMC.setTextureEnable(textureMapping);
            dispGMC.setPointSize(pointSize);

            if (xMap == null)
                return;
            xMap.setRange(ranges[0],ranges[1]);
            yMap.setRange(ranges[2],ranges[3]);
            zMap.setRange(ranges[4],ranges[5]);
            float[] range = new float[2];
            RangeControl dataRangeControl = (RangeControl)xRangeMap.getControl();
            range[0] = (float)ranges[0];
            range[1] = (float)ranges[1];
            dataRangeControl.setRange(range);
            dataRangeControl = (RangeControl)yRangeMap.getControl();
            range[0] = (float)ranges[2];
            range[1] = (float)ranges[3];
            dataRangeControl.setRange(range);
            dataRangeControl = (RangeControl)zRangeMap.getControl();
            range[0] = (float)ranges[4];
            range[1] = (float)ranges[5];
            dataRangeControl.setRange(range);
            zColorMap.setRange(ranges[6],ranges[7]);

        } catch(Exception ex){
            ex.printStackTrace();
        }
    }

    void jCheckBox1_actionPerformed(ActionEvent e) {
	try {
	    loadFile();
	} catch(Exception ex) {
	    ex.printStackTrace();
	}
    }
}
    
