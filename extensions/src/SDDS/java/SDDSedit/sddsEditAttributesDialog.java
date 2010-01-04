/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDSedit;

import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.event.*;
import SDDS.java.SDDS.*;

class sddsEditAttributesDialog extends JDialog {
    private String name = null;
    private String symbol = null;
    private String units = null;
    private String description = null;
    private String format_string = null;
    private String field_length = "0";
    private String fixed_value = null;
    private int type = 5;
    private JOptionPane optionPane;
    boolean attributes = false;
    final JTextField[] fields;
    final JRadioButton[] typeButtons;
    final JLabel[] labels;

    public void setAttributesMode(boolean parameter) {
	attributes = true;
        //name = symbol = units = description = format_string = fixed_value = null;
	//field_length = "0";
	int i;
	for (i=0;i<6;i++)
	    typeButtons[i].setEnabled(false);
	if (parameter) {
	    setTitle("Parameter Info");
	    labels[5].setText("fixed value:");
	    fields[5].setText(null);
	} else {
	    setTitle("Column Info");
	    labels[5].setText("field length:");
	    fields[5].setText("0");
	}
    }
    public void setName(String n) {
        fields[0].setText(n);
    }
    public void setSymbol(String s) {
        fields[1].setText(s);
    }
    public void setUnits(String u) {
        fields[2].setText(u);
    }
    public void setDescription(String d) {
        fields[3].setText(d);
    }
    public void setFormatString(String fs) {
        fields[4].setText(fs);
    }
    public void setFieldLength(int fl) {
        fields[5].setText(String.valueOf(fl));
    }
    public void setFixedValue(String fv) {
        fields[5].setText(fv);
    }
    public void setType(int t) {
	if ((t > 0) && (t < 7))
	    typeButtons[t-1].setSelected(true);
	else
	    typeButtons[4].setSelected(true);
    }

    public void setInsertMode(boolean parameter) {
	attributes = false;
	int i;
	for (i=0;i<6;i++)
	    typeButtons[i].setEnabled(true);
	if (parameter) {
	    setTitle("New Parameter");
	    labels[5].setText("fixed value:");
	    fields[5].setText(null);
	} else {
	    setTitle("New Column");
	    labels[5].setText("field length:");
	    fields[5].setText("0");
	}

    }
    public String getName() {
	if ((name == null) || (name.length() == 0))
	    return null;
        return name;
    }
    public String getSymbol() {
	if (symbol.length() == 0)
	    return null;
        return symbol;
    }
    public String getUnits() {
	if (units.length() == 0)
	    return null;
        return units;
    }
    public String getDescription() {
	if (description.length() == 0)
	    return null;
        return description;
    }
    public String getFormatString() {
	if (format_string.length() == 0)
	    return null;
        return format_string;
    }
    public int getFieldLength() {
	int fl;
	try {
	    fl = (Integer.valueOf(field_length)).intValue();
	} catch (NumberFormatException e) {
	    fl = 0;
	}
        return fl;
    }
    public String getFixedValue() {
	if (fixed_value.length() == 0)
	    return null;
        return fixed_value;
    }
    public int getType() {
        return type;
    }

    public sddsEditAttributesDialog(Frame aFrame) {
        super(aFrame, true);
        //final DialogDemo dd = parent;
	if (attributes)
	    setTitle("Column Info");
	else
	    setTitle("New Column");

	GridBagLayout gbl = new GridBagLayout();
	GridBagConstraints gbc = new GridBagConstraints();
	JPanel panel1 = new JPanel(gbl);

	labels = new JLabel[7];
	fields = new JTextField[6];
	labels[0] = new JLabel("name:", JLabel.LEFT);
	labels[0].setForeground(Color.black);
	gbc.ipadx = 10;
	gbc.fill = gbc.HORIZONTAL;
	gbc.weightx = 0.0;
	gbc.weighty = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[0], gbc);
	panel1.add(labels[0]);
        fields[0] = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[0], gbc);
	panel1.add(fields[0]);
	labels[1] = new JLabel("symbol:", JLabel.LEFT);
	labels[1].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[1], gbc);
	panel1.add(labels[1]);
        fields[1] = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[1], gbc);
	panel1.add(fields[1]);
	labels[2] = new JLabel("units:", JLabel.LEFT);
	labels[2].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[2], gbc);
	panel1.add(labels[2]);
        fields[2] = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[2], gbc);
	panel1.add(fields[2]);
	labels[3] = new JLabel("description:", JLabel.LEFT);
	labels[3].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[3], gbc);
	panel1.add(labels[3]);
        fields[3] = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[3], gbc);
	panel1.add(fields[3]);
	labels[4] = new JLabel("format string:", JLabel.LEFT);
	labels[4].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[4], gbc);
	panel1.add(labels[4]);
        fields[4] = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[4], gbc);
	panel1.add(fields[4]);
	labels[5] = new JLabel("field length:", JLabel.LEFT);
	labels[5].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[5], gbc);
	panel1.add(labels[5]);
        fields[5] = new JTextField("0",10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(fields[5], gbc);
	panel1.add(fields[5]);
	labels[6] = new JLabel("type:", JLabel.LEFT);
	labels[6].setForeground(Color.black);
	gbc.weightx = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(labels[6], gbc);
	panel1.add(labels[6]);

	typeButtons = new JRadioButton[6];
	typeButtons[3] = new JRadioButton("short");
	typeButtons[2] = new JRadioButton("long");
	typeButtons[1] = new JRadioButton("float");
	typeButtons[0] = new JRadioButton("double");
	typeButtons[4] = new JRadioButton("string");
	typeButtons[5] = new JRadioButton("character");
	ButtonGroup typeButtonGroup = new ButtonGroup();
	typeButtonGroup.add(typeButtons[3]);
	typeButtonGroup.add(typeButtons[2]);
	typeButtonGroup.add(typeButtons[1]);
	typeButtonGroup.add(typeButtons[0]);
	typeButtonGroup.add(typeButtons[4]);
	typeButtonGroup.add(typeButtons[5]);
	typeButtons[4].setSelected(true);

	gbc.weightx = 1.0;
	gbl.setConstraints(typeButtons[3], gbc);
	panel1.add(typeButtons[3]);
	gbl.setConstraints(typeButtons[2], gbc);
	panel1.add(typeButtons[2]);
	gbl.setConstraints(typeButtons[1], gbc);
	panel1.add(typeButtons[1]);
	gbl.setConstraints(typeButtons[0], gbc);
	panel1.add(typeButtons[0]);
	gbl.setConstraints(typeButtons[4], gbc);
	panel1.add(typeButtons[4]);
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(typeButtons[5], gbc);
	panel1.add(typeButtons[5]);

        Object[] array = {panel1};


        final String btnString1 = "OK";
        final String btnString2 = "Cancel";
        Object[] options = {btnString1, btnString2};

        optionPane = new JOptionPane(array, 
                                    JOptionPane.QUESTION_MESSAGE,
                                    JOptionPane.DEFAULT_OPTION,
                                    null,
                                    options,
                                    options[0]);
        setContentPane(optionPane);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent we) {
                /*
                 * Instead of directly closing the window,
                 * we're going to change the JOptionPane's
                 * value property.
                 */
                    optionPane.setValue(new Integer(
                                        JOptionPane.CLOSED_OPTION));
            }
        });
	/*
        fields[0].addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                optionPane.setValue(btnString1);
            }
        });
	*/
        optionPane.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                String prop = e.getPropertyName();

                if (isVisible() 
                 && (e.getSource() == optionPane)
                 && (prop.equals(JOptionPane.VALUE_PROPERTY) ||
                     prop.equals(JOptionPane.INPUT_VALUE_PROPERTY))) {
                    Object value = optionPane.getValue();

                    if (value == JOptionPane.UNINITIALIZED_VALUE) {
                        //ignore reset
                        return;
                    }

                    // Reset the JOptionPane's value.
                    // If you don't do this, then if the user
                    // presses the same button next time, no
                    // property change event will be fired.
                    optionPane.setValue(
                            JOptionPane.UNINITIALIZED_VALUE);

                    if (value.equals(btnString1)) {
			name = fields[0].getText();
			symbol = fields[1].getText();
			units = fields[2].getText();
			description = fields[3].getText();
			format_string = fields[4].getText();
			field_length = fixed_value = fields[5].getText();
			int i;
			for (i=0;i<6;i++) {
			    if (typeButtons[i].isSelected()) {
				type = i+1;
				break;
			    }
			}
                        if (SDDSUtil.isValidName(name)) {
                            // we're done; dismiss the dialog
                            setVisible(false);
                        } else { 
                            // text was invalid
                            fields[0].selectAll();
                            JOptionPane.showMessageDialog(
                                            sddsEditAttributesDialog.this,
                                            "Sorry, \"" + name + "\" "
                                            + "isn't a valid name.\n",
					    "Try again",
                                            JOptionPane.ERROR_MESSAGE);
                            name = null;
                        }
                    } else { // user closed dialog or clicked cancel
                        name = null;
                        setVisible(false);
                    }
                }
            }
        });
    }
}
