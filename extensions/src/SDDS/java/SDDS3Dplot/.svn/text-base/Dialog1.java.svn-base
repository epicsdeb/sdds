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
//import com.borland.jbcl.layout.*;
import java.awt.event.*;
import java.lang.reflect.*;

/**
 * Title:        SDDS 3D Plot
 * Description:
 * Copyright:    Copyright (c) 2001
 * Company:      ANL
 * @author Robert Soliday
 * @version 1.0
 */

public class Dialog1 extends JDialog {
    JPanel panel1 = new JPanel();
    BorderLayout borderLayout1 = new BorderLayout();
    JPanel jPanel1 = new JPanel();
    JButton jButton1 = new JButton();
    JPanel jPanel2 = new JPanel();
    JPanel jPanel3 = new JPanel();
    BorderLayout borderLayout2 = new BorderLayout();
    JButton color9 = new JButton();
    JButton color8 = new JButton();
    JButton color7 = new JButton();
    JButton color6 = new JButton();
    JButton color5 = new JButton();
    JButton color4 = new JButton();
    JButton color3 = new JButton();
    JButton color1 = new JButton();
    GridLayout gridLayout1 = new GridLayout();
    JButton color2 = new JButton();
    JSlider jSlider1 = new JSlider();
    JLabel jLabel1 = new JLabel();
    JPanel jPanel4 = new JPanel();
    GridBagLayout gridBagLayout1 = new GridBagLayout();
    JButton jButton2 = new JButton();

    JColorChooser jColorChooser1 = new JColorChooser();
    public Dialog1(Frame frame, String title, boolean modal) {
        super(frame, title, modal);
        try {
            jbInit();
            pack();
        }
        catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    public Dialog1() {
        this(null, "Select Colors", false);
    }
    void jbInit() throws Exception {
        panel1.setLayout(borderLayout1);
        jButton1.setText("Dismiss");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton1_actionPerformed(e);
            }
        });
        jPanel2.setLayout(borderLayout2);
        color9.setText("Color9");
        color9.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color8.setText("Color8");
        color8.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color7.setText("Color7");
        color7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color6.setText("Color6");
        color6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color5.setText("Color5");
        color5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color4.setText("Color4");
        color4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color3.setText("Color3");
        color3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        color1.setText("Color1");
        color1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        jPanel3.setLayout(gridLayout1);
        gridLayout1.setRows(3);
        color2.setText("Color2");
        color2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                color_actionPerformed(e);
            }
        });
        jSlider1.setMajorTickSpacing(1);
        jSlider1.setPaintLabels(true);
        jSlider1.setMinimum(1);
        jSlider1.setValue(1);
        jSlider1.setPaintTicks(true);
        jSlider1.setMaximum(9);
        jSlider1.setSnapToTicks(true);
        jLabel1.setText("Number of Colors");
        jPanel4.setLayout(gridBagLayout1);
        jButton2.setText("Apply");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(ActionEvent e) {
                jButton2_actionPerformed(e);
            }
        });
        getContentPane().add(panel1);
        panel1.add(jPanel2, BorderLayout.CENTER);
        jPanel2.add(jPanel3, BorderLayout.SOUTH);
        jPanel3.add(color1, null);
        jPanel3.add(color2, null);
        jPanel3.add(color3, null);
        jPanel3.add(color4, null);
        jPanel3.add(color5, null);
        jPanel3.add(color6, null);
        jPanel3.add(color7, null);
        jPanel3.add(color8, null);
        jPanel3.add(color9, null);
        jPanel2.add(jPanel4, BorderLayout.NORTH);
        jPanel4.add(jSlider1, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel4.add(jLabel1, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0
            ,GridBagConstraints.CENTER, GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0));
        jPanel2.add(jColorChooser1, BorderLayout.CENTER);
        this.getContentPane().add(jPanel1, BorderLayout.SOUTH);
        jPanel1.add(jButton1, null);
        jPanel1.add(jButton2, null);
    }

    void jButton1_actionPerformed(ActionEvent e) {
        setVisible(false);
    }

    void color_actionPerformed(ActionEvent e) {
        ((JButton)e.getSource()).setBackground(jColorChooser1.getColor());
    }

    void setColors(float[][] myColorTable) {
        Color[] c = new Color[9];
        int numOfColors = Array.getLength(myColorTable[0]);
        for (int i = 0; i < numOfColors; i++) {
            c[i] = new Color(myColorTable[0][i],myColorTable[1][i],myColorTable[2][i]);
        }
        for (int i = numOfColors; i < 9; i++) {
            c[i] = Color.lightGray;
        }
        color1.setBackground(c[0]);
        color2.setBackground(c[1]);
        color3.setBackground(c[2]);
        color4.setBackground(c[3]);
        color5.setBackground(c[4]);
        color6.setBackground(c[5]);
        color7.setBackground(c[6]);
        color8.setBackground(c[7]);
        color9.setBackground(c[8]);
        jSlider1.setValue(numOfColors);
    }

    void jButton2_actionPerformed(ActionEvent e) {
        int numOfColors = jSlider1.getValue();
	float[][] myColorTable = new float[3][numOfColors];
        float[][] c = new float[9][3];
        c[0] = Frame1.colorToFloats(color1.getBackground());
        c[1] = Frame1.colorToFloats(color2.getBackground());
        c[2] = Frame1.colorToFloats(color3.getBackground());
        c[3] = Frame1.colorToFloats(color4.getBackground());
        c[4] = Frame1.colorToFloats(color5.getBackground());
        c[5] = Frame1.colorToFloats(color6.getBackground());
        c[6] = Frame1.colorToFloats(color7.getBackground());
        c[7] = Frame1.colorToFloats(color8.getBackground());
        c[8] = Frame1.colorToFloats(color9.getBackground());
        for (int i=0;i<numOfColors;i++) {
            myColorTable[0][i] = c[i][0];
            myColorTable[1][i] = c[i][1];
            myColorTable[2][i] = c[i][2];
        }
        Frame1.extrapolateColors(myColorTable);
    }

}
