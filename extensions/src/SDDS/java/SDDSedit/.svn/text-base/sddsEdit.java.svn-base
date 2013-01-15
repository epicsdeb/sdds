/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDSedit;

import java.io.*;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import java.lang.reflect.*;
import java.util.*;
import java.beans.*;
import SDDS.java.SDDS.*;


public class sddsEdit extends JFrame implements Printable {
    static JTable columnTable;
    static JTable parameterTable;
    static JScrollPane columnScrollPane;
    static JScrollPane parameterScrollPane;
    static JTextArea textArea;
    static JMenu menu, submenu;
    static JMenuItem menuItem;
    static sddsEdit frame;
    private JMenuBar menubar;
    private Hashtable commands;
    public static final String newAction = "new";
    public static final String openAction = "open";
    public static final String saveAction = "save";
    public static final String saveAsAction = "saveas";
    public static final String printAction = "print";
    public static final String exitAction = "exit";
    public static final String attributesParameterAction = "attributesParameter";
    public static final String insertParameterAction = "insertParameter";
    public static final String deleteParameterAction = "deleteParameter";
    public static final String attributesColumnAction = "attributesColumn";
    public static final String insertColumnAction = "insertColumn";
    public static final String deleteColumnAction = "deleteColumn";
    public static final String insertRowAction = "insertRow";
    public static final String deleteRowAction = "deleteRow";
    public static final String goToPageAction = "goToPage";
    public static final String insertPageAction = "insertPage";
    public static final String insertAndCopyPageAction = "insertAndCopyPage";
    public static final String appendPageAction = "appendPage";
    public static final String deletePageAction = "deletePage";
    public static final String searchColumnAction = "searchColumn";
    public static final String displayInfoAction = "displayInfo";
    protected FileDialog fileDialog;
    static JRadioButton[] asciiBinaryButtons = new JRadioButton[2];
    static SDDSFile sdds;
    static JComboBox pageChooser;
    static boolean disablePageChooser=false;
    static int currentPage=0;
    static DefaultListModel column_dlm = new DefaultListModel();
    static DefaultListModel parameter_dlm = new DefaultListModel();
    static String insertRows = "1";
    static SearchDialog searchDialog = null;
    static MatchList matchList = null;

    static DefaultTableModel column_dtm, parameter_dtm;
    static DefaultTableColumnModel column_dtcm, parameter_dtcm;
    static JList parameterRowHeader;

    int [] subTableSplit = null;
    boolean pageinfoCalculated=false;
    int totalNumPages=0;
    int prevPageIndex = 0;
    int subPageIndex = 0;
    int subTableSplitSize = 0;
    int fontHeight, fontDesent;
    double tableHeightOnFullPage, headerHeight;
    double pageWidth, pageHeight;
    double tableHeight, rowHeight;

    private Action[] defaultActions = {
	new NewAction(),
	new OpenAction(),
	new SaveAction(),
	new SaveAsAction(),
	new PrintAction(),
	new ExitAction(),
	new AttributesParameterAction(),
	new InsertParameterAction(),
	new DeleteParameterAction(),
	new AttributesColumnAction(),
	new InsertColumnAction(),
	new DeleteColumnAction(),
	new InsertRowAction(),
	new DeleteRowAction(),
	new GoToPageAction(),
	new InsertPageAction(),
	new InsertAndCopyPageAction(),
	new AppendPageAction(),
	new DeletePageAction(),
	new SearchColumnAction(),
	new DisplayInfoAction()
    };

    /**
     * Fetch the list of actions supported by this
     * editor.  It is implemented to return the list
     * of actions supported by the embedded JTextComponent
     * augmented with the actions defined locally.
     */
    public Action[] getActions() {
	return defaultActions;
    }

    protected Action getAction(String cmd) {
	return (Action) commands.get(cmd);
    }

    // Yarked from JMenu, ideally this would be public.
    protected PropertyChangeListener createActionChangeListener(JMenuItem b) {
	return new ActionChangedListener(b);
    }
    // Yarked from JMenu, ideally this would be public.
    private class ActionChangedListener implements PropertyChangeListener {
        JMenuItem menuItem;
        
        ActionChangedListener(JMenuItem mi) {
            super();
            this.menuItem = mi;
        }
        public void propertyChange(PropertyChangeEvent e) {
            String propertyName = e.getPropertyName();
            if (e.getPropertyName().equals(Action.NAME)) {
                String text = (String) e.getNewValue();
                menuItem.setText(text);
            } else if (propertyName.equals("enabled")) {
                Boolean enabledState = (Boolean) e.getNewValue();
                menuItem.setEnabled(enabledState.booleanValue());
            }
        }
    }

    protected JMenuBar getMenubar() {
	return menubar;
    }
    protected JMenuBar createMenubar() {
	JMenuItem mi, sub_mi;
	JMenuBar mb = new JMenuBar();
	JMenu m, sub_m;
	m = new JMenu("File");
	mb.add(m);
	mi = new JMenuItem("New");
	SetMenuAction("new",mi);
	m.add(mi);
	mi = new JMenuItem("Open...");
	SetMenuAction("open",mi);
	m.add(mi);
	m.addSeparator();
	mi = new JMenuItem("Save");
	SetMenuAction("save",mi);
	m.add(mi);
	mi = new JMenuItem("Save as...");
	SetMenuAction("saveas",mi);
	m.add(mi);
	m.addSeparator();
	mi = new JMenuItem("Print...");
	SetMenuAction("print",mi);
	m.add(mi);
	m.addSeparator();
	mi = new JMenuItem("Quit");
	SetMenuAction("exit",mi);
	m.add(mi);
	m = new JMenu("Edit");
	mb.add(m);
	sub_m = new JMenu("Parameter");
	m.add(sub_m);
	mi = new JMenuItem("Attributes...");
	SetMenuAction("attributesParameter",mi);
	sub_m.add(mi);
	sub_m.addSeparator();
	mi = new JMenuItem("Insert...");
	SetMenuAction("insertParameter",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Delete...");
	SetMenuAction("deleteParameter",mi);
	sub_m.add(mi);
	sub_m = new JMenu("Column");
	m.add(sub_m);
	mi = new JMenuItem("Attributes...");
	SetMenuAction("attributesColumn",mi);
	sub_m.add(mi);
	sub_m.addSeparator();
	mi = new JMenuItem("Insert...");
	SetMenuAction("insertColumn",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Delete...");
	SetMenuAction("deleteColumn",mi);
	sub_m.add(mi);
	sub_m = new JMenu("Row");
	m.add(sub_m);
	mi = new JMenuItem("Insert...");
	SetMenuAction("insertRow",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Delete...");
	SetMenuAction("deleteRow",mi);
	sub_m.add(mi);
	sub_m = new JMenu("Page");
	m.add(sub_m);
	mi = new JMenuItem("Go to Page...");
	SetMenuAction("goToPage",mi);
	sub_m.add(mi);
	sub_m.addSeparator();
	mi = new JMenuItem("Insert Page");
	SetMenuAction("insertPage",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Insert and Copy Current Page");
	SetMenuAction("insertAndCopyPage",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Append Page");
	SetMenuAction("appendPage",mi);
	sub_m.add(mi);
	mi = new JMenuItem("Delete Page");
	SetMenuAction("deletePage",mi);
	sub_m.add(mi);
	m = new JMenu("Search");
	mb.add(m);
	mi = new JMenuItem("Column...");
	SetMenuAction("searchColumn",mi);
	m.add(mi);
	m = new JMenu("Info");
	mb.add(m);
	m.setActionCommand("displayInfo");
	mi = new JMenuItem("About...");
	SetMenuAction("displayInfo",mi);
	m.add(mi);
	return mb;
    }

    protected void SetMenuAction(String actionCommand, JMenuItem mi) {
	mi.setActionCommand(actionCommand);
	Action a = getAction(actionCommand);
	if (a != null) {
	    mi.addActionListener(a);
	    a.addPropertyChangeListener(createActionChangeListener(mi));
	    mi.setEnabled(a.isEnabled());
	} else {
	    mi.setEnabled(false);
	}
    }

    public sddsEdit() {
        super("SDDS Editor");


	// install the command table
	commands = new Hashtable();
	Action[] actions = getActions();
	for (int i = 0; i < actions.length; i++) {
	    Action a = actions[i];
	    commands.put(a.getValue(Action.NAME), a);
	}


	menubar = createMenubar();
	getContentPane().add(menubar, BorderLayout.NORTH);
	
	JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
	splitPane.setOneTouchExpandable(true);
	getContentPane().add(splitPane, BorderLayout.CENTER);

        textArea = new JTextArea();
        textArea.setFont(new Font("Courier", Font.PLAIN, 12));
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);
	textArea.setEditable(false);
	
        JScrollPane areaScrollPane = new JScrollPane(textArea);
        areaScrollPane.setVerticalScrollBarPolicy(
                        JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        areaScrollPane.setPreferredSize(new Dimension(640, 80));

	splitPane.setTopComponent(areaScrollPane);

	JPanel panel1 = new JPanel(new BorderLayout());
	splitPane.setBottomComponent(panel1);
	
	JSplitPane dataPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
	dataPane.setOneTouchExpandable(true);
	panel1.add(dataPane, BorderLayout.CENTER);

	GridBagLayout gridbag = new GridBagLayout();
	GridBagConstraints c = new GridBagConstraints();

	JPanel panel2 = new JPanel(gridbag);
	dataPane.setTopComponent(panel2);

	JLabel pageChooserLabel = new JLabel("Page", JLabel.CENTER);
	pageChooserLabel.setForeground(Color.black);
	c.ipadx = 10;
	c.weightx = 0.0;
	c.gridwidth = 1;
	gridbag.setConstraints(pageChooserLabel, c);
	panel2.add(pageChooserLabel);

	pageChooser = new JComboBox();
	c.ipadx = 0;
	gridbag.setConstraints(pageChooser, c);
	panel2.add(pageChooser);
	pageChooser.addItem(new Integer(1));

	pageChooser.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    if (!disablePageChooser) {
			JComboBox cb = (JComboBox)e.getSource();
			int page = cb.getSelectedIndex();
			if (currentPage != page) {
			    if (savePage()) {
				clearPage();
				loadPage(page+1);
			    } else
				pageChooser.setSelectedIndex(currentPage);
			}	
		    }
		}
	    });
	
	JLabel parameterLabel = new JLabel("Parameters", JLabel.CENTER);
	parameterLabel.setForeground(Color.black);
	c.weightx = 1.0;
	gridbag.setConstraints(parameterLabel, c);
	panel2.add(parameterLabel);

	asciiBinaryButtons[0] = new JRadioButton("ascii");
	asciiBinaryButtons[1] = new JRadioButton("binary");
	ButtonGroup dataTypeButtonGroup = new ButtonGroup();
	dataTypeButtonGroup.add(asciiBinaryButtons[0]);
	dataTypeButtonGroup.add(asciiBinaryButtons[1]);
	c.weightx = 0.0;
	gridbag.setConstraints(asciiBinaryButtons[0], c);
	panel2.add(asciiBinaryButtons[0]);
	c.gridwidth = c.REMAINDER;
	gridbag.setConstraints(asciiBinaryButtons[1], c);
	panel2.add(asciiBinaryButtons[1]);
	asciiBinaryButtons[0].setSelected(true);

	
	parameterTable = new JTable(new DefaultTableModel())
	    {
		protected void configureEnclosingScrollPane() {
		    Container p = getParent();
		    if (p instanceof JViewport) {
			Container gp = p.getParent();
			if (gp instanceof JScrollPane) {
			    JScrollPane scrollPane = (JScrollPane)gp;
			    // Make certain we are the 
			    // viewPort's view and not, for
			    // example, the rowHeaderView of the scrollPane -
			    // an implementor of fixed columns might do this.
			    JViewport viewport = scrollPane.getViewport();
			    if (viewport==null || viewport.getView()!=this) {
				return;
			    }
			    //scrollPane.setColumnHeaderView(getTableHeader());
			    scrollPane.getViewport().setBackingStoreEnabled(true);
			    scrollPane.setBorder(UIManager.getBorder("Table.scrollPaneBorder"));
			}
		    }
		}
	    };
	parameterTable.setColumnSelectionAllowed(false);
	parameterTable.setPreferredScrollableViewportSize(new Dimension(240, 100));
	parameterTable.setAutoResizeMode(JTable.AUTO_RESIZE_LAST_COLUMN);
	
        parameterScrollPane = new JScrollPane(parameterTable);
	
	parameterRowHeader = new JList(parameter_dlm); 
	parameterRowHeader.setBackground(parameterTable.getTableHeader().getBackground());   
	//parameterRowHeader.setFixedCellHeight(parameterTable.getRowHeight() + parameterTable.getRowMargin());
	parameterRowHeader.setFixedCellHeight(parameterTable.getRowHeight());
	parameterRowHeader.setCellRenderer(new RowHeaderRenderer("parameter"));
	parameterScrollPane.setRowHeaderView(parameterRowHeader);
	
	
	c.fill = c.BOTH;
	c.weightx = 1.0;
	c.weighty = 0.2;
	gridbag.setConstraints(parameterScrollPane, c);
	
        panel2.add(parameterScrollPane);

	JPanel panel3 = new JPanel(gridbag);
	dataPane.setBottomComponent(panel3);

	JLabel columnLabel = new JLabel("Columns", JLabel.CENTER);
	columnLabel.setForeground(Color.black);
	c.fill = c.HORIZONTAL;
	c.weighty = 0.0;
	c.gridx = 2;
	c.gridy = 3;
	c.gridwidth = 1;
	gridbag.setConstraints(columnLabel, c);
	panel3.add(columnLabel);
	
	columnTable = new JTable(new DefaultTableModel());
	columnTable.setCellSelectionEnabled(true);
	columnTable.setPreferredScrollableViewportSize(new Dimension(240, 150));
	columnTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
	
        columnScrollPane = new JScrollPane(columnTable);

	JList rowHeader;
	rowHeader = new JList(column_dlm); 
	rowHeader.setBackground(columnTable.getTableHeader().getBackground());   
	rowHeader.setFixedCellWidth(50);
	//rowHeader.setFixedCellHeight(columnTable.getRowHeight() + columnTable.getRowMargin());
	rowHeader.setFixedCellHeight(columnTable.getRowHeight());
	rowHeader.setCellRenderer(new RowHeaderRenderer("column"));
	columnScrollPane.setRowHeaderView(rowHeader);
	
	c.fill = c.BOTH;
	c.weighty = 1.0;
	c.gridx = 0;
	c.gridy = 4;
	c.gridwidth = c.REMAINDER;
	c.gridheight = c.REMAINDER;
	gridbag.setConstraints(columnScrollPane, c);
	
        panel3.add(columnScrollPane);



	column_dtm = (DefaultTableModel)columnTable.getModel();
	parameter_dtm = (DefaultTableModel)parameterTable.getModel();
	column_dtcm = (DefaultTableColumnModel)columnTable.getColumnModel();
	parameter_dtcm = (DefaultTableColumnModel)parameterTable.getColumnModel();

        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });
        sdds = new SDDSFile();
    }

    protected static void appendStatus(String s) {
	textArea.append(s + "\n");
	textArea.setCaretPosition(textArea.getDocument().getLength());
    }

    public static void main(String[] args) {
	String[][] commandLineArgs;
	String initFile=null;
	int arguments;
	frame = new sddsEdit();
        frame.pack();
        frame.setVisible(true);
	appendStatus("Ready...");
        frame.setTitle("SDDS Editor");

	commandLineArgs = SDDSUtil.parseCommandLine(args);
	arguments = Array.getLength(commandLineArgs);
	if (arguments==1) {
	    initFile = commandLineArgs[0][0];
	    frame.loadFile(initFile);
	}
    }

    public void loadFile(String fileName) {
	int pages;
	int i;
	sdds = new SDDSFile(fileName);
	if (!sdds.readFile()) {
	    appendStatus("Error: " + sdds.getErrors());
	    return;
        }
	clearPage();
	loadPage(1);
	disablePageChooser = true;
	pageChooser.setSelectedIndex(-1);
	pageChooser.removeAllItems();
	pages = sdds.pageCount();
	pageChooser.addItem(new Integer(1));
	for (i=2;i<=pages;i++)
	    pageChooser.addItem(new Integer(i));
	disablePageChooser = false;

	if (sdds.asciiFile)
	    asciiBinaryButtons[0].setSelected(true);
	else
	    asciiBinaryButtons[1].setSelected(true);
	appendStatus(fileName + " loaded");
        frame.setTitle("SDDS Editor - " + fileName);
    }

    public void clearPage() {
	if (columnTable.isEditing()) {
	    columnTable.getCellEditor().stopCellEditing();
	}
	if (parameterTable.isEditing()) {
	    parameterTable.getCellEditor().stopCellEditing();
	}
	columnTable.getSelectionModel().clearSelection();
	columnTable.getColumnModel().getSelectionModel().clearSelection();
	parameterTable.getSelectionModel().clearSelection();
	column_dtm.setNumRows(0);
	column_dtm.setDataVector(new Object[0][0], new Object[0]);
	parameter_dtm.setNumRows(0);
        parameter_dtm.setDataVector(new Object[0][0], new Object[0]);
    }

    public void loadPage(int page) {
	String[] columnNames=null, parameterNames=null;
	Object[][] columnValues=null;
	Object[][] columnMajorOrderValues=null;
	Object[][] parameterValues=null;
	int i, j, numberOfColumns=0, numberOfParameters=0;
	int rows;
	DefaultCellEditor dce;
	parameterNames = sdds.getParameterNames();
	if (parameterNames != null) {
	    numberOfParameters = Array.getLength(parameterNames);
	    parameterValues = new Object[numberOfParameters][1];
	    parameter_dlm.clear();
	    for (i=0;i<numberOfParameters;i++) {
		parameterValues[i][0] = sdds.getParameterValue(i,page,false);
		parameter_dlm.addElement(parameterNames[i]);
	    }
	    
	    parameter_dtm.setDataVector(parameterValues, new Object[1]);

	    //parameterTable.getTableHeader().setVisible(false);
	    
	    if ((DefaultCellEditor)(parameter_dtcm.getColumn(0).getCellEditor()) == null) {
		dce = new DefaultCellEditor(new JTextField());
		dce.setClickCountToStart(1);
		parameter_dtcm.getColumn(0).setCellEditor((TableCellEditor)dce);
	    }
	    parameterTable.sizeColumnsToFit(JTable.AUTO_RESIZE_LAST_COLUMN);
	    
	}

	columnNames = sdds.getColumnNames();
	if (columnNames != null) {
	    numberOfColumns = Array.getLength(columnNames);
	    rows = sdds.getRowCount(page);
	    columnValues = new Object[numberOfColumns][rows];
	    columnMajorOrderValues = new Object[rows][numberOfColumns];
	    for (i=0;i<numberOfColumns;i++)
		columnValues[i] = sdds.getColumnValues(i,page,false);
	    column_dlm.clear();
	    for (i=0;i<rows;i++) {
		column_dlm.addElement(new Integer(i+1));
		for (j=0;j<numberOfColumns;j++) {
		    columnMajorOrderValues[i][j] = columnValues[j][i];
		}
	    }
	    column_dtm.setDataVector(columnMajorOrderValues, columnNames);
	    ButtonHeaderRenderer renderer = new ButtonHeaderRenderer();
	    TableColumn tc;
	    for (i=0;i<numberOfColumns;i++) {
		tc = column_dtcm.getColumn(i);
		if (columnNames[i].length() <= 7) {
		    tc.setPreferredWidth(75);
		    tc.setWidth(75);
		} else {
		    tc.setPreferredWidth((columnNames[i].length() + 1) * 10);
		    tc.setWidth((columnNames[i].length() + 1) * 10);
		}
		tc.setHeaderRenderer(renderer);
		if ((DefaultCellEditor)(column_dtcm.getColumn(i).getCellEditor()) == null) {
		    dce = new DefaultCellEditor(new JTextField());
		    dce.setClickCountToStart(1);
		    column_dtcm.getColumn(i).setCellEditor((TableCellEditor)dce);
		}
	    }
	    JTableHeader header = columnTable.getTableHeader();
	    header.addMouseListener(new HeaderListener(header,renderer));
	    //columnTable.sizeColumnsToFit(JTable.AUTO_RESIZE_ALL_COLUMNS);

	}
	currentPage = page-1;
	appendStatus("page #" + page + " loaded");
    }

    class HeaderListener extends MouseAdapter {
	JTableHeader   header;
	ButtonHeaderRenderer renderer;
	
	HeaderListener(JTableHeader header,ButtonHeaderRenderer renderer) {
	    this.header   = header;
	    this.renderer = renderer;
	}
	
	public void mousePressed(MouseEvent e) {
	    int col = header.columnAtPoint(e.getPoint());
	    renderer.setPressedColumn(col);
	    header.repaint();
	    if (columnTable.isEditing()) {
		columnTable.getCellEditor().stopCellEditing();
	    }
	    clearColumnSelections();
	    columnTable.setColumnSelectionInterval(col,col);
	    columnTable.getSelectionModel().addSelectionInterval(0, column_dtm.getRowCount() - 1);
	}
	
	public void mouseReleased(MouseEvent e) {
	    int col = header.columnAtPoint(e.getPoint());
	    renderer.setPressedColumn(-1);                // clear
	    header.repaint();
	}
    }

    
    class ButtonHeaderRenderer extends JButton implements TableCellRenderer {
	int pushedColumn;
	
	public ButtonHeaderRenderer() {
	    pushedColumn   = -1;
	    setMargin(new Insets(0,0,0,0));
	}
	
	public Component getTableCellRendererComponent(JTable table, Object value,
						       boolean isSelected, boolean hasFocus, int row, int column) {
	    setText((value ==null) ? "" : value.toString());
	    boolean isPressed = (column == pushedColumn);
	    getModel().setPressed(isPressed);
	    getModel().setArmed(isPressed);
	    return this;
	}
	
	public void setPressedColumn(int col) {
	    pushedColumn = col;
	}
    }

    public boolean savePage() {
	String[] columnNames=null, parameterNames=null;
	String[][] columnValues=null;
	String[] parameterValues=null;
	int i, j, numberOfColumns=0, numberOfParameters;
	int rows, cols, pars;

	if (columnTable.isEditing()) {
	    columnTable.getCellEditor().stopCellEditing();
	}
	if (parameterTable.isEditing()) {
	    parameterTable.getCellEditor().stopCellEditing();
	}
	columnTable.getSelectionModel().clearSelection();
	columnTable.getColumnModel().getSelectionModel().clearSelection();
	parameterTable.getSelectionModel().clearSelection();
 	parameterNames = sdds.getParameterNames();
	if (parameterNames != null) {
	    numberOfParameters = Array.getLength(parameterNames);
	    pars = parameter_dlm.getSize(); /* may be different then numberOfColumns */
	    parameterValues = new String[pars];

	    int[] types = new int[numberOfParameters];
	    for (i=0;i<pars;i++) {
		try { 
		    parameterValues[i] = parameter_dtm.getValueAt(i,0).toString();
		} catch (Exception e) {
		    parameterValues[i] = "";
		}
		types[i] = sdds.getParameterType(i);
	    }
	    for (i=0;i<numberOfParameters;i++) {
		for (j=0;j<pars;j++) {
		    if (parameterNames[i].equals((String)parameter_dlm.getElementAt(j))) {
			if (types[i] == SDDSUtil.SDDS_CHARACTER) {
			    if (!sdds.setParameter(i,SDDSUtil.returnCharValueFromOctalCode(parameterValues[j]),currentPage+1)) {
				appendStatus("Error: " + sdds.getErrors());
				return false;
			    }
			} else {
			    if (!sdds.setParameter(i,parameterValues[j],currentPage+1)) {
				appendStatus("Error: " + sdds.getErrors());
				return false;
			    }
			}
			break;
		    }
		}
	    }
	}
 	columnNames = sdds.getColumnNames();
	if (columnNames != null) {
	    numberOfColumns = Array.getLength(columnNames);
	    rows = column_dtm.getRowCount();
	    cols = column_dtm.getColumnCount(); /* may be different then numberOfColumns */
	    columnValues = new String[cols][rows];
	    
	    for (j=0;j<rows;j++) {
		for (i=0;i<cols;i++) {
		    columnValues[i][j] = column_dtm.getValueAt(j,i).toString();
		}
	    }
	    for (i=0;i<numberOfColumns;i++) {
		for (j=0;j<cols;j++) {
		    if (columnNames[i].equals(column_dtm.getColumnName(j))) {
			if (!sdds.setColumn(i,columnValues[j],currentPage+1,1)) {
			    appendStatus("Error: " + sdds.getErrors());
			    return false;
			}
			break;
		    }
		}
	    }
	}
	return true;
    }


    class NewAction extends AbstractAction {

	NewAction() {
	    super(newAction);
	}

	NewAction(String nm) {
	    super(nm);
	}

        public void actionPerformed(ActionEvent e) {
	    appendStatus("starting new file");
	    sdds = new SDDSFile();
	    clearPage();
	    disablePageChooser = true;
	    pageChooser.setSelectedIndex(-1);
	    pageChooser.removeAllItems();
	    pageChooser.addItem(new Integer(1));
	    disablePageChooser = false;
	    column_dlm.clear();
	    parameter_dlm.clear();
	    autoSearch();
	}
    }

    class OpenAction extends NewAction {

	OpenAction() {
	    super(openAction);
	}

        public void actionPerformed(ActionEvent e) {
	    if (fileDialog == null) {
		fileDialog = new FileDialog(frame);
	    }
	    fileDialog.setMode(FileDialog.LOAD);
	    fileDialog.show();

	    String file = fileDialog.getFile();
	    if (file == null) {
		return;
	    }
	    String directory = fileDialog.getDirectory();
	    File f = new File(directory, file);
	    if (f.exists()) {
		appendStatus("loading " + f.getAbsolutePath());

		if (columnTable.isEditing()) {
		    columnTable.getCellEditor().stopCellEditing();
		}
		if (parameterTable.isEditing()) {
		    parameterTable.getCellEditor().stopCellEditing();
		}
		columnTable.getSelectionModel().clearSelection();
		columnTable.getColumnModel().getSelectionModel().clearSelection();
		parameterTable.getSelectionModel().clearSelection();

		loadFile(f.getAbsolutePath());
	    }
	    autoSearch();
	}
    }


    class SaveAction extends AbstractAction {

	SaveAction() {
	    super(saveAction);
	}

        public void actionPerformed(ActionEvent e) {
	    savePage();
	    if (sdds.fileName == null) {
		if (fileDialog == null) {
		    fileDialog = new FileDialog(frame);
		}
		fileDialog.setMode(FileDialog.SAVE);
		fileDialog.show();
		
		String file = fileDialog.getFile();
		if (file == null) {
		    return;
		}
		String directory = fileDialog.getDirectory();
		File f = new File(directory, file);
		sdds.setFileName(f.getPath());
	    }
	    if (asciiBinaryButtons[1].isSelected()) {
		sdds.setMode(true);
	    } else {
		sdds.setMode(false);
	    }
	    if (!sdds.writeFile()) {
		appendStatus("Error: " + sdds.getErrors());
		return;
	    }
	    appendStatus("data saved to " + sdds.fileName);
	    frame.setTitle("SDDS Editor - " + sdds.fileName);
	}
    }

    class SaveAsAction extends AbstractAction {

	SaveAsAction() {
	    super(saveAsAction);
	}

        public void actionPerformed(ActionEvent e) {
	    savePage();
	    if (fileDialog == null) {
		fileDialog = new FileDialog(frame);
	    }
	    fileDialog.setMode(FileDialog.SAVE);
	    fileDialog.show();

	    String file = fileDialog.getFile();
	    if (file == null) {
		return;
	    }
	    String directory = fileDialog.getDirectory();
	    File f = new File(directory, file);
	    sdds.setFileName(f.getPath());
	    if (asciiBinaryButtons[1].isSelected()) {
		sdds.setMode(true);
	    } else {
		sdds.setMode(false);
	    }
	    if (!sdds.writeFile()) {
		appendStatus("Error: " + sdds.getErrors());
		return;
	    }
	    appendStatus("data saved to " + sdds.fileName);
	    frame.setTitle("SDDS Editor - " + file);
	}
    }

    class PrintAction extends AbstractAction {

	PrintAction() {
	    super(printAction);
	}

        public void actionPerformed(ActionEvent e) {
	    PrinterJob pj = PrinterJob.getPrinterJob();
	    pj.setPrintable(sddsEdit.this);
	    if (pj.printDialog()) {
		try {
		    pj.print();
		} catch (Exception PrintException) {} {
		}
	    }
	}
    }

    class ExitAction extends AbstractAction {

	ExitAction() {
	    super(exitAction);
	}

        public void actionPerformed(ActionEvent e) {
	    System.exit(0);
	}
    }

    class AttributesParameterAction extends AbstractAction {

	AttributesParameterAction() {
	    super(attributesParameterAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int i, j, k;
	    i = parameterTable.getSelectedRow();
	    if (i >= 0) {
		TableColumn tc;
		String name = (String)parameter_dlm.getElementAt(i);
		int index = sdds.getParameterIndex(name);
		if (index < 0) {
		    appendStatus("Error: " + sdds.getErrors());
		    return;
		}
		sddsEditAttributesDialog sddsEditAttributesDialog = new sddsEditAttributesDialog(frame);
		sddsEditAttributesDialog.pack();	
		sddsEditAttributesDialog.setAttributesMode(true);
		sddsEditAttributesDialog.setName(name);
		sddsEditAttributesDialog.setSymbol(sdds.getParameterSymbol(index));
		sddsEditAttributesDialog.setUnits(sdds.getParameterUnits(index));
		sddsEditAttributesDialog.setDescription(sdds.getParameterDescription(index));
		sddsEditAttributesDialog.setFormatString(sdds.getParameterFormatString(index));
		sddsEditAttributesDialog.setType(sdds.getParameterType(index));
		sddsEditAttributesDialog.setFixedValue(sdds.getParameterFixedValue(index));
		
		sddsEditAttributesDialog.setLocationRelativeTo(frame);
		sddsEditAttributesDialog.setVisible(true);
		if (sddsEditAttributesDialog.getName() == null)
		    return;	
		sdds.setParameterSymbol(index, sddsEditAttributesDialog.getSymbol());
		sdds.setParameterUnits(index, sddsEditAttributesDialog.getUnits());
		sdds.setParameterDescription(index, sddsEditAttributesDialog.getDescription());
		sdds.setParameterFormatString(index, sddsEditAttributesDialog.getFormatString());
		sdds.setParameterFixedValue(index, sddsEditAttributesDialog.getFixedValue());
		sdds.renameParameter(name, sddsEditAttributesDialog.getName());
		parameter_dlm.set(i,sddsEditAttributesDialog.getName());
	    }
	}
    }

    class InsertParameterAction extends AbstractAction {

	InsertParameterAction() {
	    super(insertParameterAction);
	}

        public void actionPerformed(ActionEvent e) {
	    sddsEditAttributesDialog sddsEditAttributesDialog = new sddsEditAttributesDialog(frame);
	    sddsEditAttributesDialog.pack();	
	    sddsEditAttributesDialog.setInsertMode(true);
	    sddsEditAttributesDialog.setLocationRelativeTo(frame);
	    sddsEditAttributesDialog.setVisible(true);
	    if (sddsEditAttributesDialog.getName() == null)
		return;
	    if (sdds.defineParameter(sddsEditAttributesDialog.getName(), sddsEditAttributesDialog.getSymbol(),
				     sddsEditAttributesDialog.getUnits(), sddsEditAttributesDialog.getDescription(),
				     sddsEditAttributesDialog.getFormatString(), sddsEditAttributesDialog.getType(),
				     sddsEditAttributesDialog.getFixedValue()) == -1) {
		appendStatus("Error: " + sdds.getErrors());
		return;
	    }
	    int pages, i;
	    String[] emptyData = new String[1];
	    String emptyString = "";
	    DefaultCellEditor dce;
	    emptyData[0] = emptyString;

	    pages = sdds.pageCount();
	    if (pages > 0) {
		emptyData = new String[pages];
		for (i=0;i<pages;i++)
		    emptyData[i] = emptyString;
		if (!sdds.setParameter(sddsEditAttributesDialog.getName(),emptyData,1)) {
		    appendStatus("Error: " + sdds.getErrors());
		    return;
		}
	    }
	    String[] parameterNames=null;
	    int numberOfParameters=0;
	    Object[][] parameterValues=null;
	    parameterNames = sdds.getParameterNames();
	    numberOfParameters = Array.getLength(parameterNames);
	    parameterValues = new Object[numberOfParameters][1];
	    parameter_dlm.clear();
	    for (i=0;i<numberOfParameters;i++) {
		parameterValues[i][0] = sdds.getParameterValue(i,currentPage+1,false);
		parameter_dlm.addElement(parameterNames[i]);		
	    }
	    parameter_dtm.setDataVector(parameterValues, new Object[1]);
	    if ((DefaultCellEditor)(parameter_dtcm.getColumn(0).getCellEditor()) == null) {
		dce = new DefaultCellEditor(new JTextField());
		dce.setClickCountToStart(1);
		parameter_dtcm.getColumn(0).setCellEditor((TableCellEditor)dce);
	    }
	    parameterTable.sizeColumnsToFit(JTable.AUTO_RESIZE_LAST_COLUMN);
	    
	    

	    appendStatus("parameter added");
	}
    }

    class DeleteParameterAction extends AbstractAction {

	DeleteParameterAction() {
	    super(deleteParameterAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int selectedRowCount;
	    selectedRowCount = parameterTable.getSelectedRowCount();
	    if (selectedRowCount > 0) {
		int i;
		String[] emptyData = new String[1];
		String emptyString = "";
		int[] selectedIndexes = new int[selectedRowCount];

		emptyData[0] = emptyString;
		selectedIndexes = parameterTable.getSelectedRows();
		if (parameterTable.isEditing())
		    parameterTable.getCellEditor().stopCellEditing();

		for (i=selectedRowCount-1;i>=0;i--) {
		    sdds.deleteParameter((String)parameter_dlm.getElementAt(selectedIndexes[i]));
		    parameter_dlm.remove(selectedIndexes[i]);
		    parameter_dtm.removeRow(selectedIndexes[i]);
		    appendStatus("parameter removed");
		}
	    }
	}
    }

    class AttributesColumnAction extends AbstractAction {

	AttributesColumnAction() {
	    super(attributesColumnAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int i, j, k;
	    i = columnTable.getSelectedColumn();
	    if (i >= 0) {
		String name = columnTable.getColumnName(i);
		int index = sdds.getColumnIndex(name);
		if (index < 0) {
		    appendStatus("Error: " + sdds.getErrors());
		    return;
		}
		sddsEditAttributesDialog sddsEditAttributesDialog = new sddsEditAttributesDialog(frame);
		sddsEditAttributesDialog.pack();	

		sddsEditAttributesDialog.setAttributesMode(false);
		sddsEditAttributesDialog.setName(name);
		sddsEditAttributesDialog.setSymbol(sdds.getColumnSymbol(index));
		sddsEditAttributesDialog.setUnits(sdds.getColumnUnits(index));
		sddsEditAttributesDialog.setFormatString(sdds.getColumnFormatString(index));
		sddsEditAttributesDialog.setType(sdds.getColumnType(index));
		sddsEditAttributesDialog.setFieldLength(sdds.getColumnFieldLength(index));
		
		sddsEditAttributesDialog.setLocationRelativeTo(frame);
		sddsEditAttributesDialog.setVisible(true);
		if (sddsEditAttributesDialog.getName() == null)
		    return;
		sdds.setColumnSymbol(index, sddsEditAttributesDialog.getSymbol());
		sdds.setColumnUnits(index, sddsEditAttributesDialog.getUnits());
		sdds.setColumnDescription(index, sddsEditAttributesDialog.getDescription());
		sdds.setColumnFormatString(index, sddsEditAttributesDialog.getFormatString());
		sdds.setColumnFieldLength(index, sddsEditAttributesDialog.getFieldLength());
		sdds.renameColumn(name, sddsEditAttributesDialog.getName());
		String[] columnNames=null;
		j = column_dtm.getColumnCount();
	        columnNames = new String[j];
		TableColumn tc;
		int width;
		int[] widtharray = new int[j];
		for (k=0;k<j;k++) {
		    if (i!=k)
			columnNames[k] = column_dtm.getColumnName(k);
		    else
			columnNames[k] = sddsEditAttributesDialog.getName();
		    tc = column_dtcm.getColumn(k);
		    widtharray[k] = tc.getWidth();
		}
		column_dtm.setColumnIdentifiers(columnNames);
		for (k=0;k<j;k++) {
		    tc = column_dtcm.getColumn(k);
		    if (k==i) {
			width = (columnNames[k].length() + 1) * 10;
			if (width > widtharray[k])
			    widtharray[k] = width;
		    }
		    tc.setPreferredWidth(widtharray[k]);
		    tc.setWidth(widtharray[k]);
		}
	    }
	}
    }

    class InsertColumnAction extends AbstractAction {

	InsertColumnAction() {
	    super(insertColumnAction);
	}

        public void actionPerformed(ActionEvent e) {
	    sddsEditAttributesDialog sddsEditAttributesDialog = new sddsEditAttributesDialog(frame);
	    sddsEditAttributesDialog.pack();	
	    sddsEditAttributesDialog.setInsertMode(false);
	    sddsEditAttributesDialog.setLocationRelativeTo(frame);
	    sddsEditAttributesDialog.setVisible(true);
	    if (sddsEditAttributesDialog.getName() == null)
		return;
	    int rows, i, j, k, pages, num;
	    String[] emptyData;
	    String emptyString = "";
	    rows = column_dtm.getRowCount();
	    if (sdds.defineColumn(sddsEditAttributesDialog.getName(), sddsEditAttributesDialog.getSymbol(), 
				  sddsEditAttributesDialog.getUnits(), sddsEditAttributesDialog.getDescription(),
				  sddsEditAttributesDialog.getFormatString(), sddsEditAttributesDialog.getType(),
				  sddsEditAttributesDialog.getFieldLength()) == -1) {
		appendStatus("Error: " + sdds.getErrors());
		return;
	    }
	    emptyData = new String[rows];
	    for (i=0;i<rows;i++)
		emptyData[i] = emptyString;
	    column_dtm.addColumn(sddsEditAttributesDialog.getName(), emptyData);
	    ButtonHeaderRenderer renderer = new ButtonHeaderRenderer();
	    TableColumn tc;
	    int numberOfColumns = column_dtcm.getColumnCount();
	    String[] columnNames = sdds.getColumnNames();
	    DefaultCellEditor dce;
	    for (i=0;i<numberOfColumns;i++) {
		tc = column_dtcm.getColumn(i);
		if (columnNames[i].length() <= 7) {
		    tc.setPreferredWidth(75);
		    tc.setWidth(75);
		} else {
		    tc.setPreferredWidth((columnNames[i].length() + 1) * 10);
		    tc.setWidth((columnNames[i].length() + 1) * 10);
		}
		tc.setHeaderRenderer(renderer);
		if ((DefaultCellEditor)(column_dtcm.getColumn(i).getCellEditor()) == null) {
		    dce = new DefaultCellEditor(new JTextField());
		    dce.setClickCountToStart(1);
		    column_dtcm.getColumn(i).setCellEditor((TableCellEditor)dce);
		}
	    }
	    JTableHeader header = columnTable.getTableHeader();
	    header.addMouseListener(new HeaderListener(header,renderer));
	    pages = sdds.pageCount();
	    for (i=1;i<=pages;i++) {
		rows = sdds.getRowCount(i);
		if (rows > 0) {
		    emptyData = new String[rows];
		    for (j=0;j<rows;j++)
			emptyData[j] = emptyString;
		    if (!sdds.setColumn(sddsEditAttributesDialog.getName(),emptyData,i,1)) {
			appendStatus("Error: " + sdds.getErrors());
			return;
		    }
		}
	    }
	    appendStatus("column added");
	}
    }

    class DeleteColumnAction extends AbstractAction {

	DeleteColumnAction() {
	    super(deleteColumnAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int i;
	    TableColumn tc;
	    i = columnTable.getSelectedColumn();
	    while (i >= 0) {
		tc = columnTable.getColumn(columnTable.getColumnName(i));
		sdds.deleteColumn(columnTable.getColumnName(i));
		columnTable.removeColumn(tc);
		appendStatus("column removed");
		i = columnTable.getSelectedColumn();
	    }
	}
    }

    class InsertRowAction extends AbstractAction {

	InsertRowAction() {
	    super(insertRowAction);
	}
	
        public void actionPerformed(ActionEvent e) {
	    int selectedRowCount;
	    selectedRowCount = columnTable.getSelectedRowCount();
	    if ((selectedRowCount > 0) || (columnTable.getRowCount() == 0)) {
		TextInsertDialog textInsertDialog = new TextInsertDialog(frame);
		textInsertDialog.pack();
		textInsertDialog.setTitleInfo("Insert Rows");
		textInsertDialog.setEntryLabel("Number of new rows:");
		textInsertDialog.setEntryText(insertRows);
		textInsertDialog.setLocationRelativeTo(frame);
		textInsertDialog.setVisible(true);
		
		int rows;
		insertRows = textInsertDialog.getText();
		if (insertRows == null)
		    return;
		try {
		    rows = (Integer.valueOf(insertRows)).intValue();
		} catch (NumberFormatException ex) {
		    return;
		}
		if (rows > 0) {
		    
		    int cols, i;
		    String[] emptyData=null;
		    String emptyString = "";
		    int selectedIndex;
		    
		    cols = column_dtm.getColumnCount();
		    emptyData = new String[cols];
		    for (i=0;i<cols;i++)
			emptyData[i] = emptyString;
		    selectedIndex = columnTable.getSelectedRow();
		    if (selectedIndex == -1)
			selectedIndex = 0;
		    if (columnTable.isEditing()) {
			columnTable.getCellEditor().stopCellEditing();
		    }
		    
		    for (i=0;i<rows;i++) {
			column_dtm.insertRow(selectedIndex,emptyData);
			column_dlm.addElement(new Integer(column_dtm.getRowCount()));
		    }
		    if (rows == 1)
			appendStatus("Row added");
		    else
			appendStatus("Rows added");
		}
	    }
	    autoSearch();
	}
    }

    class DeleteRowAction extends AbstractAction {

	DeleteRowAction() {
	    super(deleteRowAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int selectedRowCount;
	    selectedRowCount = columnTable.getSelectedRowCount();
	    if (selectedRowCount > 0) {
		int cols, i;
		String[] emptyData=null;
		String emptyString = "";
		int selectedIndexes[];
		
		cols = column_dtm.getColumnCount();
		emptyData = new String[cols];
		for (i=0;i<cols;i++)
		    emptyData[i] = emptyString;
		selectedIndexes = new int[selectedRowCount];
		selectedIndexes = columnTable.getSelectedRows();
		if (columnTable.isEditing()) {
		    columnTable.getCellEditor().stopCellEditing();
		}
		
		for (i=selectedRowCount-1;i>=0;i--) {
		    sdds.deleteRow(currentPage+1, selectedIndexes[i]+1);
		    column_dtm.removeRow(selectedIndexes[i]);
		}
		
		column_dlm.clear();
		int rows;
		rows = column_dtm.getRowCount();
		for (i=0;i<rows;i++) {
		    column_dlm.addElement(new Integer(i+1));
		}
		if (selectedRowCount == 1)
		    appendStatus("Row deleted");
		else
		    appendStatus("Rows deleted");
	    }
	    autoSearch();
	}
    }

    class GoToPageAction extends AbstractAction {

	GoToPageAction() {
	    super(goToPageAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int pages;
	    pages = sdds.pageCount();
	    if (pages > 1) {
		TextInsertDialog textInsertDialog = new TextInsertDialog(frame);
		textInsertDialog.pack();
		textInsertDialog.setTitleInfo("Go To Page");
		textInsertDialog.setEntryLabel("Page number from 1 to " + pages);
		textInsertDialog.setEntryText(String.valueOf(currentPage+1));
		textInsertDialog.setLocationRelativeTo(frame);
		textInsertDialog.setVisible(true);
		
		int page;
		String dialogOutput;
		dialogOutput = textInsertDialog.getText();
		if (dialogOutput == null)
		    return;
		try {
		    page = (Integer.valueOf(dialogOutput)).intValue();
		} catch (NumberFormatException ex) {
		    return;
		}
		if ((page > 0) && (page <= pages)) {
		    if (currentPage != page-1) {
			disablePageChooser = true;
			if (savePage()) {
			    clearPage();
			    loadPage(page);
			    currentPage = page - 1;
			}
			pageChooser.setSelectedIndex(currentPage);
			disablePageChooser = false;
		    }	
		}
	    }
	    autoSearch();
	}
    }

    class InsertPageAction extends AbstractAction {

	InsertPageAction() {
	    super(insertPageAction);
	}

        public void actionPerformed(ActionEvent e) {
	    String[] columnNames=null, parameterNames=null;
	    parameterNames = sdds.getParameterNames();
	    columnNames = sdds.getColumnNames();
	    if ((parameterNames == null) || (columnNames == null)) {
		appendStatus("Unable to insert page");
	    } else if ((Array.getLength(parameterNames) == 0) && (Array.getLength(columnNames) == 0)) {
		appendStatus("Define parameters or columns first");
	    } else {
		if (savePage()) {
		    clearPage();
		    if (!sdds.insertPage(currentPage+1)) {
			appendStatus("Error: " + sdds.getErrors());
			return;
		    }
		    loadPage(currentPage+1);
		    pageChooser.addItem(new Integer(sdds.pageCount()));
		    appendStatus("Page inserted");
		}
	    }
	    autoSearch();
	}
    }

    class InsertAndCopyPageAction extends AbstractAction {

	InsertAndCopyPageAction() {
	    super(insertAndCopyPageAction);
	}

        public void actionPerformed(ActionEvent e) {
	    String[] columnNames=null, parameterNames=null;
	    parameterNames = sdds.getParameterNames();
	    columnNames = sdds.getColumnNames();
	    if ((parameterNames == null) || (columnNames == null)) {
		appendStatus("Unable to insert page");
	    } else if ((Array.getLength(parameterNames) == 0) && (Array.getLength(columnNames) == 0)) {
		appendStatus("Define parameters or columns first");
	    } else {
		if (savePage()) {
		    if (!sdds.insertPage(currentPage+2)) {
			appendStatus("qError: " + sdds.getErrors());
			return;
		    }
		    currentPage++;
		    if (savePage()) {
		    }
		    currentPage--;
		    pageChooser.addItem(new Integer(sdds.pageCount()));
		    appendStatus("Page inserted and copied");
		}
	    }
	    autoSearch();
	}
    }

    class AppendPageAction extends AbstractAction {

	AppendPageAction() {
	    super(appendPageAction);
	}

        public void actionPerformed(ActionEvent e) {
	    String[] columnNames=null, parameterNames=null;
	    parameterNames = sdds.getParameterNames();
	    columnNames = sdds.getColumnNames();
	    if ((parameterNames == null) || (columnNames == null)) {
		appendStatus("Unable to append page");
	    } else if ((Array.getLength(parameterNames) == 0) && (Array.getLength(columnNames) == 0)) {
		appendStatus("Define parameters or columns first");
	    } else {
		if (savePage()) {
		    if (!sdds.insertPage(currentPage+2)) {
			appendStatus("Error: " + sdds.getErrors());
			return;
		    }
		    pageChooser.addItem(new Integer(sdds.pageCount()));
		    appendStatus("Page appended");
		}
	    }
	    autoSearch();
	}
    }

    class DeletePageAction extends AbstractAction {

	DeletePageAction() {
	    super(deletePageAction);
	}

        public void actionPerformed(ActionEvent e) {
	    int pages;
	    pages = sdds.pageCount();
	    if (pages == 1) {
		if (!sdds.deletePage(currentPage+1)) {
		    appendStatus("Error: " + sdds.getErrors());
		    return;
		}
		clearPage();
		loadPage(currentPage+1);
	       
	    } else {
		if (!sdds.deletePage(currentPage+1)) {
		    appendStatus("Error: " + sdds.getErrors());
		    return;
		}
		disablePageChooser = true;
		
		clearPage();
		pages--;
		if (pages == currentPage)
		    currentPage--;
		loadPage(currentPage+1);
		
		pageChooser.removeAllItems();
		pageChooser.addItem(new Integer(1));
		int i;
		for (i=2;i<=pages;i++)
		    pageChooser.addItem(new Integer(i));
		pageChooser.setSelectedIndex(currentPage);
		disablePageChooser = false;
	    }
	    autoSearch();
	}
    }

    class SearchColumnAction extends AbstractAction {

	SearchColumnAction() {
	    super(searchColumnAction);
	}

        public void actionPerformed(ActionEvent e) {
	    String[] columnNames=null;
	    boolean[] columnMatch=null;
	    int cols, rows, i, j, index=0;
	    cols = column_dtm.getColumnCount();
	    rows = column_dtm.getRowCount();
	    if (cols < 1)
		return;
	    columnNames = new String[cols];
	    for (i=0;i<cols;i++)
		columnNames[i] = column_dtm.getColumnName(i);
	    if (searchDialog == null)
		searchDialog = new SearchDialog(frame);
	    searchDialog.addColumns(columnNames);
	    searchDialog.pack();
	    searchDialog.setLocationRelativeTo(frame);
	    searchDialog.setVisible(true);
	    String column, pattern;
	    column = searchDialog.getName();
	    if (column == null)
		return;	
	    pattern = searchDialog.getPattern();
	    if (pattern == null)
		return;

	    if (columnTable.isEditing()) {
		columnTable.getCellEditor().stopCellEditing();
	    }
	    for (i=0;i<cols;i++) {
		if (columnNames[i].equals(column)) {
		    index = i;
		    break;
		}
	    }
	    int matches = 0;
	    columnMatch = new boolean[rows];
	    for (i=0;i<rows;i++) {
		if (columnMatch[i] = SDDSUtil.globMatch(column_dtm.getValueAt(i,index).toString(), pattern))
		    matches++;
	    }
	    String[] elements = new String[matches];
	    j = 0;
	    for (i=0;i<rows;i++) {
		if (columnMatch[i]) {
		    elements[j] = column_dtm.getValueAt(i,index).toString() + "@" + (i+1) + "," + (index+1);
		    j++;
		}
	    }
	    if (matchList == null)
		matchList = new MatchList();
	    matchList.setElements(elements);
	    matchList.pack();
	    matchList.setVisible(true);
	}
    }

    public static void autoSearch() {
	if (matchList == null)
	    return;
	if (matchList.isVisible() == false)
	    return;
	String[] columnNames=null;
	boolean[] columnMatch=null;
	int cols, rows, i, j, index=0;
	String column, pattern;
	cols = column_dtm.getColumnCount();
	rows = column_dtm.getRowCount();
	if (cols < 1)
	    return;
	columnNames = new String[cols];
	for (i=0;i<cols;i++)
	    columnNames[i] = column_dtm.getColumnName(i);

	if (searchDialog == null)
	    searchDialog = new SearchDialog(frame);
	column = searchDialog.getName();
	if (column == null)
	    return;	
	pattern = searchDialog.getPattern();
	if (pattern == null)
	    return;
	
	if (columnTable.isEditing()) {
	    columnTable.getCellEditor().stopCellEditing();
	}
	for (i=0;i<cols;i++) {
	    if (columnNames[i].equals(column)) {
		index = i;
		break;
	    }
	}
	int matches = 0;
	columnMatch = new boolean[rows];
	for (i=0;i<rows;i++) {
	    if (columnMatch[i] = SDDSUtil.globMatch(column_dtm.getValueAt(i,index).toString(), pattern))
		matches++;
	}
	String[] elements = new String[matches];
	j = 0;
	for (i=0;i<rows;i++) {
	    if (columnMatch[i]) {
		elements[j] = column_dtm.getValueAt(i,index).toString() + "@" + (i+1) + "," + (index+1);
		j++;
	    }
	}
	matchList.setElements(elements);
	matchList.pack();
    }

    public static void clearColumnSelections() {
	columnTable.getSelectionModel().clearSelection();	
    }

    public static void selectColumnElement(int row, int col) {
	columnTable.setColumnSelectionInterval(col,col);
	columnTable.getSelectionModel().addSelectionInterval(row,row);
    }

    public static void scrollToColumnElement(int row, int col) {
	Rectangle theRect = columnTable.getCellRect(row,col,false);
	columnTable.scrollRectToVisible(theRect);
    }

    public static void deleteColumnRow(int row) {
	if (columnTable.isEditing()) {
	    columnTable.getCellEditor().stopCellEditing();
	}

	sdds.deleteRow(currentPage+1, row+1);
	column_dtm.removeRow(row);
	
	column_dlm.clear();
	int rows, i;
	rows = column_dtm.getRowCount();
	for (i=0;i<rows;i++) {
	    column_dlm.addElement(new Integer(i+1));
	}
	appendStatus("Row deleted");
    }

    class DisplayInfoAction extends AbstractAction {

	DisplayInfoAction() {
	    super(displayInfoAction);
	}

        public void actionPerformed(ActionEvent e) {
	    InfoDialog infoDialog = new InfoDialog(frame);
	    infoDialog.pack();
	    infoDialog.setLocationRelativeTo(frame);
	    infoDialog.setVisible(true);
	}

    }

    class RowHeaderRenderer extends JButton implements ListCellRenderer {
	int pushedRow;
	boolean paraTable = false;
	RowHeaderRenderer(String tabletype) {
	    JTableHeader header;
	    if (tabletype.equals("parameter")) {
		paraTable = true;
		header = parameterTable.getTableHeader();		
	    } else {
		header = columnTable.getTableHeader();
	    }
	    setOpaque(true);
	    setBorder(UIManager.getBorder("TableHeader.cellBorder"));
	    setHorizontalAlignment(CENTER);
	    setForeground(header.getForeground());
	    setBackground(header.getBackground());
	    setFont(header.getFont());
	    pushedRow = -1;
	    setMargin(new Insets(0,0,0,0));
	}
	public Component getListCellRendererComponent(JList list, Object value,
						      int index, 
						      boolean isSelected, 
						      boolean cellHasFocus) {
	    setText((value == null) ? "" : value.toString());
	    boolean isPressed = false;
	    if (isSelected && cellHasFocus) {
		isPressed = true;
		if (paraTable) {
		    if (parameterTable.isEditing()) {
			parameterTable.getCellEditor().stopCellEditing();
		    }
		    parameterTable.getSelectionModel().clearSelection();
		    parameterTable.getSelectionModel().setSelectionInterval(index,index);
		    parameterTable.setColumnSelectionInterval(0, 0);
		} else {
		    if (columnTable.isEditing()) {
			columnTable.getCellEditor().stopCellEditing();
		    }
		    columnTable.getSelectionModel().clearSelection();
		    columnTable.getSelectionModel().setSelectionInterval(index,index);
		    columnTable.setColumnSelectionInterval(0, ((DefaultTableModel)columnTable.getModel()).getColumnCount() - 1);
		}
	    }
	    getModel().setPressed(isPressed);
	    getModel().setArmed(isPressed);
	    return this;
	}
    }
    
    public int printold(Graphics g, PageFormat pageFormat, 
		     int pageIndex) throws PrinterException {
        Graphics2D  g2 = (Graphics2D) g;
        g2.setColor(Color.black);
        int fontHeight=g2.getFontMetrics().getHeight();
        int fontDesent=g2.getFontMetrics().getDescent();
	
        //leave room for page number
        double pageHeight = pageFormat.getImageableHeight()-fontHeight;
        double pageWidth = pageFormat.getImageableWidth();
        double tableWidth = (double) columnTable.getColumnModel().getTotalColumnWidth();
        double scale = 1; 
        if (tableWidth >= pageWidth) {
	    scale =  pageWidth / tableWidth;
        }
	
        double headerHeightOnPage=
	    columnTable.getTableHeader().getHeight()*scale;
        double tableWidthOnPage=tableWidth*scale;
	
        double oneRowHeight=(columnTable.getRowHeight()+
			     columnTable.getRowMargin())*scale;
        int numRowsOnAPage=
	    (int)((pageHeight-headerHeightOnPage)/oneRowHeight);
        double pageHeightForTable=oneRowHeight*numRowsOnAPage;
        int totalNumPages= (int)Math.ceil((
					   (double)columnTable.getRowCount())/numRowsOnAPage);
        if(pageIndex>=totalNumPages) {
	    return NO_SUCH_PAGE;
        }
	
        g2.translate(pageFormat.getImageableX(), 
		     pageFormat.getImageableY());
        g2.drawString("Page: "+(pageIndex+1),(int)pageWidth/2-35,
                      (int)(pageHeight+fontHeight-fontDesent));//bottom center
	
        g2.translate(0f,headerHeightOnPage);
        g2.translate(0f,-pageIndex*pageHeightForTable);
	
        //If this piece of the table is smaller than the size available,
        //clip to the appropriate bounds.
        if (pageIndex + 1 == totalNumPages) {
	    int lastRowPrinted = numRowsOnAPage * pageIndex;
	    int numRowsLeft = columnTable.getRowCount() - lastRowPrinted;
	    g2.setClip(0, (int)(pageHeightForTable * pageIndex),
                       (int) Math.ceil(tableWidthOnPage),
                       (int) Math.ceil(oneRowHeight * numRowsLeft));
        }
        //else clip to the entire area available.
        else{    
	    g2.setClip(0, (int)(pageHeightForTable*pageIndex), 
		       (int) Math.ceil(tableWidthOnPage),
		       (int) Math.ceil(pageHeightForTable));        
        }
	
        g2.scale(scale,scale);
        columnTable.paint(g2);
        g2.scale(1/scale,1/scale);
        g2.translate(0f,pageIndex*pageHeightForTable);
        g2.translate(0f, -headerHeightOnPage);
        g2.setClip(0, 0,(int) Math.ceil(tableWidthOnPage), 
		   (int)Math.ceil(headerHeightOnPage));
        g2.scale(scale,scale);
        columnTable.getTableHeader().paint(g2);//paint header at top
	
        return Printable.PAGE_EXISTS;
    }

    public int print(Graphics g, PageFormat pageFormat, int pageIndex) throws PrinterException {
	
        Graphics2D g2=(Graphics2D)g;
	
        if(!pageinfoCalculated) {
	    getPageInfo(g, pageFormat);
        }
	
        g2.setColor(Color.black);
        if (pageIndex>=totalNumPages) {
	    return NO_SUCH_PAGE;
        }
	if (prevPageIndex != pageIndex) {
	    subPageIndex++;
	    if ( subPageIndex == subTableSplitSize -1) {
		subPageIndex=0;
	    }
	}
	
        g2.translate(pageFormat.getImageableX(), pageFormat.getImageableY());
	
        int rowIndex = pageIndex/ (subTableSplitSize -1);
        
        printTablePart(g2, pageFormat, rowIndex, subPageIndex);
        prevPageIndex= pageIndex;
	
        return Printable.PAGE_EXISTS;
    }

    public void getPageInfo(Graphics g, PageFormat pageFormat) {
	
        subTableSplit = null;
        subTableSplitSize = 0;
        subPageIndex = 0;
        prevPageIndex = 0;
	
        fontHeight=g.getFontMetrics().getHeight();
        fontDesent=g.getFontMetrics().getDescent();
	
	JTableHeader header = columnTable.getTableHeader();
        double headerWidth = header.getWidth();
        headerHeight = header.getHeight() + columnTable.getRowMargin();
	
        pageHeight = pageFormat.getImageableHeight();
        pageWidth =  pageFormat.getImageableWidth();
	
        double tableWidth = columnTable.getColumnModel().getTotalColumnWidth();
        tableHeight = columnTable.getHeight();
        rowHeight = columnTable.getRowHeight() + columnTable.getRowMargin();
	
        tableHeightOnFullPage = (int)(pageHeight - headerHeight - fontHeight*2);
        tableHeightOnFullPage = tableHeightOnFullPage/rowHeight * rowHeight;
	
        TableColumnModel tableColumnModel = header.getColumnModel();
        int columns = tableColumnModel.getColumnCount();
        int columnMargin = tableColumnModel.getColumnMargin();
	
        int [] temp = new int[columns];
        int columnIndex = 0;
        temp[0] = 0;
        int columnWidth;
        int length = 0;
        subTableSplitSize = 0;
        while ( columnIndex < columns ) {
	    
	    columnWidth = tableColumnModel.getColumn(columnIndex).getWidth();
	    
	    if ( length + columnWidth + columnMargin > pageWidth ) {
		temp[subTableSplitSize+1] = temp[subTableSplitSize] + length;
		length = columnWidth;
		subTableSplitSize++;
            }
            else {
		length += columnWidth + columnMargin;
            }
            columnIndex++;
        } //while
	
        if ( length > 0 )  {  // if are more columns left, part page
	    temp[subTableSplitSize+1] = temp[subTableSplitSize] + length;
	    subTableSplitSize++;
        }
	
        subTableSplitSize++;
        subTableSplit = new int[subTableSplitSize];
        for ( int i=0; i < subTableSplitSize; i++ ) {
	    subTableSplit[i]= temp[i];
        }
        totalNumPages = (int)(tableHeight/tableHeightOnFullPage);
        if ( tableHeight%tableHeightOnFullPage >= rowHeight ) { // at least 1 more row left
            totalNumPages++;
        }
	
        totalNumPages *= (subTableSplitSize-1);
        pageinfoCalculated = true;
    }
    
    public void printTablePart(Graphics2D g2, PageFormat pageFormat, int rowIndex, int columnIndex) {
	
        String pageNumber = "Page: "+(rowIndex+1);
        if ( subTableSplitSize > 1 ) {
	    pageNumber += "-" + (columnIndex+1);
        }
	
        int pageLeft = subTableSplit[columnIndex];
        int pageRight = subTableSplit[columnIndex + 1];
	
        int pageWidth =  pageRight-pageLeft;
        // page number message
        g2.drawString(pageNumber,  pageWidth/2-35, (int)(pageHeight - fontHeight));
	
        double clipHeight = Math.min(tableHeightOnFullPage, tableHeight - rowIndex*tableHeightOnFullPage);
	
        g2.translate(-subTableSplit[columnIndex], 0);
        g2.setClip(pageLeft ,0, pageWidth, (int)headerHeight);
	
	JTableHeader header = columnTable.getTableHeader();
        header.paint(g2);   // draw the header on every page
        g2.translate(0, headerHeight);
        g2.translate(0,  -tableHeightOnFullPage*rowIndex);
	
        // cut table image and draw on the page
	
        g2.setClip(pageLeft, (int)tableHeightOnFullPage*rowIndex, pageWidth, (int)clipHeight);
        columnTable.paint(g2);
	
        double pageTop =  tableHeightOnFullPage*rowIndex - headerHeight;
        double pageBottom = pageTop +  clipHeight + headerHeight;
        g2.drawRect(pageLeft, (int)pageTop, pageWidth, (int)(clipHeight+ headerHeight));
    }
}
