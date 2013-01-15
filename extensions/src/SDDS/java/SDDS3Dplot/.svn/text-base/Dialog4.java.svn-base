/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDS3Dplot;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import visad.java3d.ProjectionControlJ3D;

/**
 * Title:        SDDS 3D Plot
 * Description:
 * Copyright:    Copyright (c) 2001
 * Company:      ANL
 * @author Robert Soliday
 * @version 1.0
 */

public class Dialog4 extends JDialog {
    JPanel panel1 = new JPanel();
    JPanel jPanel1 = new JPanel();
    JCheckBox textureMapping = new JCheckBox();
    JCheckBox pointMode = new JCheckBox();
    JCheckBox enableScale = new JCheckBox();
    JPanel jPanel2 = new JPanel();
    JLabel jLabel2 = new JLabel();
    JTextField pointSize = new JTextField();
    JPanel jPanel3 = new JPanel();
    TitledBorder titledBorder1;
    JButton jButton1 = new JButton();
    JButton jButton2 = new JButton();
    JPanel jPanel4 = new JPanel();
    TitledBorder titledBorder2;
    JLabel jLabel3 = new JLabel();
    JLabel jLabel4 = new JLabel();
    JLabel jLabel5 = new JLabel();
    JLabel jLabel6 = new JLabel();
    JTextField colormaximum = new JTextField();
    JTextField colorminimum = new JTextField();
    JTextField zmaximum = new JTextField();
    JTextField zminimum = new JTextField();
    JTextField ymaximum = new JTextField();
    JTextField yminimum = new JTextField();
    JTextField xmaximum = new JTextField();
    JTextField xminimum = new JTextField();
    GridBagLayout gridBagLayout1 = new GridBagLayout();
    GridBagLayout gridBagLayout2 = new GridBagLayout();
    JPanel jPanel5 = new JPanel();
    JButton jButton3 = new JButton();
    JButton jButton4 = new JButton();
    JButton jButton5 = new JButton();
    GridBagLayout gridBagLayout3 = new GridBagLayout();
    JButton jButton6 = new JButton();
    JButton jButton7 = new JButton();
    JButton jButton8 = new JButton();
    TitledBorder titledBorder3;

    public Dialog4(Frame frame, String title, boolean modal) {
        super(frame, title, modal);
        try {
            jbInit();
            pack();
        }
        catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    public Dialog4() {
        this(null, "Controls", false);
    }
    void jbInit() throws Exception {
        titledBorder1 = new TitledBorder("");
        titledBorder2 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(134, 134, 134)),"Range");
        titledBorder3 = new TitledBorder(BorderFactory.createEtchedBorder(Color.white,new Color(134, 134, 134)),"Orientation");
        panel1.setLayout(gridBagLayout2);
        textureMapping.setText("Texture mapping");
        pointMode.setText("Point mode");
        enableScale.setSelected(true);
        enableScale.setText("Enable scale");
        jLabel2.setText("Point size:");
        pointSize.setMargin(new Insets(0, 3, 0, 0));
        pointSize.setText("1");
        pointSize.setColumns(5);
        jButton1.setText("Dismiss");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton1_actionPerformed(e);
            }
        });
        jButton2.setText("Apply");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton2_actionPerformed(e);
            }
        });
        jPanel4.setBorder(titledBorder2);
        jPanel4.setLayout(gridBagLayout1);
        jLabel3.setHorizontalAlignment(SwingConstants.TRAILING);
        jLabel3.setText("X");
        jLabel4.setHorizontalAlignment(SwingConstants.TRAILING);
        jLabel4.setText("Y");
        jLabel5.setHorizontalAlignment(SwingConstants.TRAILING);
        jLabel5.setText("Z");
        jLabel6.setHorizontalAlignment(SwingConstants.TRAILING);
        jLabel6.setText("Color");
        colormaximum.setMargin(new Insets(0, 3, 0, 0));
        colormaximum.setColumns(15);
        colorminimum.setMargin(new Insets(0, 3, 0, 0));
        colorminimum.setColumns(15);
        zmaximum.setMargin(new Insets(0, 3, 0, 0));
        zmaximum.setColumns(15);
        zminimum.setMargin(new Insets(0, 3, 0, 0));
        zminimum.setColumns(15);
        ymaximum.setMargin(new Insets(0, 3, 0, 0));
        ymaximum.setColumns(15);
        yminimum.setMargin(new Insets(0, 3, 0, 0));
        yminimum.setColumns(15);
        xmaximum.setMargin(new Insets(0, 3, 0, 0));
        xmaximum.setColumns(15);
        xminimum.setMargin(new Insets(0, 3, 0, 0));
        xminimum.setColumns(15);
        jButton3.setText("X");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton3_actionPerformed(e);
            }
        });
        jButton4.setText("-Y");
        jButton4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton4_actionPerformed(e);
            }
        });
        jButton5.setText("Z");
        jButton5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton5_actionPerformed(e);
            }
        });
        jPanel5.setLayout(gridBagLayout3);
        jButton6.setText("-Z");
        jButton6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton6_actionPerformed(e);
            }
        });
        jButton7.setText("Y");
        jButton7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton7_actionPerformed(e);
            }
        });
        jButton8.setText("-X");
        jButton8.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton8_actionPerformed(e);
            }
        });
        jPanel5.setBorder(titledBorder3);
        getContentPane().add(panel1);
        panel1.add(jPanel1, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel1.add(enableScale, null);
        jPanel1.add(pointMode, null);
        jPanel1.add(textureMapping, null);
        panel1.add(jPanel2, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel2.add(jLabel2, null);
        jPanel2.add(pointSize, null);
        panel1.add(jPanel4, new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(jLabel3, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(xminimum, new GridBagConstraints(1, 0, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 5, 0, 0), 0, 0));
        jPanel4.add(xmaximum, new GridBagConstraints(2, 0, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 1), 0, 0));
        jPanel4.add(jLabel4, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(yminimum, new GridBagConstraints(1, 1, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 5, 0, 0), 0, 0));
        jPanel4.add(ymaximum, new GridBagConstraints(2, 1, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 1), 0, 0));
        jPanel4.add(jLabel5, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(zminimum, new GridBagConstraints(1, 2, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 5, 0, 0), 0, 0));
        jPanel4.add(zmaximum, new GridBagConstraints(2, 2, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 1), 0, 0));
        jPanel4.add(jLabel6, new GridBagConstraints(0, 3, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(colorminimum, new GridBagConstraints(1, 3, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 5, 0, 0), 0, 0));
        jPanel4.add(colormaximum, new GridBagConstraints(2, 3, 1, 1, 1.0, 0.0
            ,GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(0, 0, 0, 1), 0, 0));
        panel1.add(jPanel3, new GridBagConstraints(0, 3, 1, 1, 1.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 2, 0), 0, 0));
        jPanel3.add(jButton1, null);
        jPanel3.add(jButton2, null);
        panel1.add(jPanel5, new GridBagConstraints(1, 0, 1, 3, 0.0, 0.0
            ,GridBagConstraints.NORTHWEST, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton6, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton7, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton8, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton5, new GridBagConstraints(1, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton4, new GridBagConstraints(1, 3, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
        jPanel5.add(jButton3, new GridBagConstraints(2, 2, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0, 0, 0, 0), 0, 0));
    }

    void getDisplayProperties(boolean es, boolean pm,
                              boolean tm, float ps,
                              double[] xRange, double[] yRange,
                              double[] zRange, double[] zColorRange) {
        enableScale.setSelected(es);
        pointMode.setSelected(pm);
        textureMapping.setSelected(tm);
        pointSize.setText(Float.toString(ps));
        xminimum.setText(Double.toString(xRange[0]));
        xmaximum.setText(Double.toString(xRange[1]));
        yminimum.setText(Double.toString(yRange[0]));
        ymaximum.setText(Double.toString(yRange[1]));
        zminimum.setText(Double.toString(zRange[0]));
        zmaximum.setText(Double.toString(zRange[1]));
        colorminimum.setText(Double.toString(zColorRange[0]));
        colormaximum.setText(Double.toString(zColorRange[1]));
    }

    void jButton1_actionPerformed(ActionEvent e) {
        setVisible(false);
    }

    void jButton2_actionPerformed(ActionEvent e) {
        double[] ranges = new double[8];
        ranges[0] = Double.parseDouble(xminimum.getText());
        ranges[1] = Double.parseDouble(xmaximum.getText());
        ranges[2] = Double.parseDouble(yminimum.getText());
        ranges[3] = Double.parseDouble(ymaximum.getText());
        ranges[4] = Double.parseDouble(zminimum.getText());
        ranges[5] = Double.parseDouble(zmaximum.getText());
        ranges[6] = Double.parseDouble(colorminimum.getText());
        ranges[7] = Double.parseDouble(colormaximum.getText());

        Frame1.setDisplayProperties(enableScale.isSelected(),
                                    pointMode.isSelected(),
                                    textureMapping.isSelected(),
                                    Float.parseFloat(pointSize.getText()),
                                    ranges);
    }

    void jButton6_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.Z_MINUS);
    }

    void jButton5_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.Z_PLUS);
    }

    void jButton4_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.Y_MINUS);
    }

    void jButton7_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.Y_PLUS);
    }

    void jButton8_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.X_MINUS);
    }

    void jButton3_actionPerformed(ActionEvent e) {
        Frame1.setOrthoView(ProjectionControlJ3D.X_PLUS);
    }
}
