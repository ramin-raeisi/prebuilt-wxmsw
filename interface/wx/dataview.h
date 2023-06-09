/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.h
// Purpose:     interface of wxDataView* classes
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    @class wxDataViewModel

    wxDataViewModel is the base class for all data model to be displayed by a
    wxDataViewCtrl.

    All other models derive from it and must implement its pure virtual functions
    in order to define a complete data model. In detail, you need to override
    wxDataViewModel::IsContainer, wxDataViewModel::GetParent, wxDataViewModel::GetChildren,
    wxDataViewModel::GetColumnCount, wxDataViewModel::GetColumnType and
    wxDataViewModel::GetValue in order to define the data model which acts as an
    interface between your actual data and the wxDataViewCtrl.

    Note that wxDataViewModel does not define the position or index of any item
    in the control because different controls might display the same data differently.
    wxDataViewModel does provide a wxDataViewModel::Compare method which the
    wxDataViewCtrl may use to sort the data either in conjunction with a column
    header or without (see wxDataViewModel::HasDefaultCompare).

    wxDataViewModel (as indeed the entire wxDataViewCtrl code) is using wxVariant
    to store data and its type in a generic way. wxVariant can be extended to contain
    almost any data without changes to the original class. To a certain extent,
    you can use (the somewhat more elegant) wxAny instead of wxVariant as there
    is code to convert between the two, but it is unclear what impact this will
    have on performance.

    Since you will usually allow the wxDataViewCtrl to change your data
    through its graphical interface, you will also have to override
    wxDataViewModel::SetValue which the wxDataViewCtrl will call when a change
    to some data has been committed.

    If the data represented by the model is changed by something else than its
    associated wxDataViewCtrl, the control has to be notified about the change.
    Depending on what happened you need to call one of the following methods:
    - wxDataViewModel::ValueChanged,
    - wxDataViewModel::ItemAdded,
    - wxDataViewModel::ItemDeleted,
    - wxDataViewModel::ItemChanged,
    - wxDataViewModel::Cleared.

    There are plural forms for notification of addition, change or removal of
    several item at once. See:
    - wxDataViewModel::ItemsAdded,
    - wxDataViewModel::ItemsDeleted,
    - wxDataViewModel::ItemsChanged.

    This class maintains a list of wxDataViewModelNotifier which link this class
    to the specific implementations on the supported platforms so that e.g. calling
    wxDataViewModel::ValueChanged on this model will just call
    wxDataViewModelNotifier::ValueChanged for each notifier that has been added.
    You can also add your own notifier in order to get informed about any changes
    to the data in the list model.

    Currently wxWidgets provides the following models apart from the base model:
    wxDataViewIndexListModel, wxDataViewVirtualListModel, wxDataViewTreeStore,
    wxDataViewListStore.

    Note that wxDataViewModel is reference counted, derives from wxRefCounter
    and cannot be deleted directly as it can be shared by several wxDataViewCtrls.
    This implies that you need to decrease the reference count after
    associating the model with a control like this:

    @code
        wxDataViewCtrl *musicCtrl = new wxDataViewCtrl( this, wxID_ANY );
        wxDataViewModel *musicModel = new MyMusicModel;
        m_musicCtrl->AssociateModel( musicModel );
        musicModel->DecRef();  // avoid memory leak !!

        // add columns now
    @endcode

    A potentially better way to avoid memory leaks is to use wxObjectDataPtr
    
    @code
        wxObjectDataPtr<MyMusicModel> musicModel;
        
        wxDataViewCtrl *musicCtrl = new wxDataViewCtrl( this, wxID_ANY );
        musicModel = new MyMusicModel;
        m_musicCtrl->AssociateModel( musicModel.get() );

        // add columns now
    @endcode


    @library{wxadv}
    @category{dvc}
*/
class wxDataViewModel : public wxRefCounter
{
public:
    /**
        Constructor.
    */
    wxDataViewModel();

    /**
        Adds a wxDataViewModelNotifier to the model.
    */
    void AddNotifier(wxDataViewModelNotifier* notifier);

    /**
        Change the value of the given item and update the control to reflect
        it.

        This function simply calls SetValue() and, if it succeeded,
        ValueChanged().

        @since 2.9.1

        @param variant
            The new value.
        @param item
            The item (row) to update.
        @param col
            The column to update.
        @return
            @true if both SetValue() and ValueChanged() returned @true.
     */
    bool ChangeValue(const wxVariant& variant,
                     const wxDataViewItem& item,
                     unsigned int col);

    /**
        Called to inform the model that all data has been cleared.
        The control will reread the data from the model again.
    */
    virtual bool Cleared();

    /**
        The compare function to be used by control. The default compare function
        sorts by container and other items separately and in ascending order.
        Override this for a different sorting behaviour.

        @see HasDefaultCompare().
    */
    virtual int Compare(const wxDataViewItem& item1,
                        const wxDataViewItem& item2,
                        unsigned int column,
                        bool ascending) const;

    /**
        Override this to indicate that the item has special font attributes.
        This only affects the wxDataViewTextRendererText renderer.

        The base class version always simply returns @false.

        @see wxDataViewItemAttr.

        @param item
            The item for which the attribute is requested.
        @param col
            The column of the item for which the attribute is requested.
        @param attr
            The attribute to be filled in if the function returns @true.
        @return
            @true if this item has an attribute or @false otherwise.
    */
    virtual bool GetAttr(const wxDataViewItem& item, unsigned int col,
                         wxDataViewItemAttr& attr) const;

    /**
        Override this to indicate that the item should be disabled.

        Disabled items are displayed differently (e.g. grayed out) and cannot
        be interacted with.

        The base class version always returns @true, thus making all items
        enabled by default.

        @param item
            The item whose enabled status is requested.
        @param col
            The column of the item whose enabled status is requested.
        @return
            @true if this item should be enabled, @false otherwise.

        @note Currently disabling items is not supported by the wxOSX/Carbon
              implementation.

        @since 2.9.2
    */
    virtual bool IsEnabled(const wxDataViewItem &item,
                           unsigned int col) const;

    /**
        Override this so the control can query the child items of an item.
        Returns the number of items.
    */
    virtual unsigned int GetChildren(const wxDataViewItem& item,
                                     wxDataViewItemArray& children) const = 0;

    /**
        Override this to indicate the number of columns in the model.
    */
    virtual unsigned int GetColumnCount() const = 0;

    /**
        Override this to indicate what type of data is stored in the
        column specified by @a col.

        This should return a string indicating the type of data as reported by wxVariant.
    */
    virtual wxString GetColumnType(unsigned int col) const = 0;

    /**
        Override this to indicate which wxDataViewItem representing the parent
        of @a item or an invalid wxDataViewItem if the root item is
        the parent item.
    */
    virtual wxDataViewItem GetParent(const wxDataViewItem& item) const = 0;

    /**
        Override this to indicate the value of @a item.
        A wxVariant is used to store the data.
    */
    virtual void GetValue(wxVariant& variant, const wxDataViewItem& item,
                          unsigned int col) const = 0;

    /**
        Override this method to indicate if a container item merely acts as a
        headline (or for categorisation) or if it also acts a normal item with
        entries for further columns. By default returns @false.
    */
    virtual bool HasContainerColumns(const wxDataViewItem& item) const;

    /**
        Override this to indicate that the model provides a default compare
        function that the control should use if no wxDataViewColumn has been
        chosen for sorting. Usually, the user clicks on a column header for
        sorting, the data will be sorted alphanumerically.

        If any other order (e.g. by index or order of appearance) is required,
        then this should be used.
        See wxDataViewIndexListModel for a model which makes use of this.
    */
    virtual bool HasDefaultCompare() const;

    /**
        Return true if there is a value in the given column of this item.

        All normal items have values in all columns but the container items
        only show their label in the first column (@a col == 0) by default (but
        see HasContainerColumns()). So this function always returns true for
        the first column while for the other ones it returns true only if the
        item is not a container or HasContainerColumns() was overridden to
        return true for it.

        @since 2.9.1
     */
    bool HasValue(const wxDataViewItem& item, unsigned col) const;

    /**
        Override this to indicate of @a item is a container, i.e. if
        it can have child items.
    */
    virtual bool IsContainer(const wxDataViewItem& item) const = 0;

    /**
        Call this to inform the model that an item has been added to the data.
    */
    bool ItemAdded(const wxDataViewItem& parent,
                           const wxDataViewItem& item);

    /**
        Call this to inform the model that an item has changed.

        This will eventually emit a @c wxEVT_DATAVIEW_ITEM_VALUE_CHANGED
        event (in which the column fields will not be set) to the user.
    */
    bool ItemChanged(const wxDataViewItem& item);

    /**
        Call this to inform the model that an item has been deleted from the data.
    */
    bool ItemDeleted(const wxDataViewItem& parent,
                             const wxDataViewItem& item);

    /**
        Call this to inform the model that several items have been added to the data.
    */
    bool ItemsAdded(const wxDataViewItem& parent,
                            const wxDataViewItemArray& items);

    /**
        Call this to inform the model that several items have changed.

        This will eventually emit @c wxEVT_DATAVIEW_ITEM_VALUE_CHANGED
        events (in which the column fields will not be set) to the user.
    */
    bool ItemsChanged(const wxDataViewItemArray& items);

    /**
        Call this to inform the model that several items have been deleted.
    */
    bool ItemsDeleted(const wxDataViewItem& parent,
                              const wxDataViewItemArray& items);

    /**
        Remove the @a notifier from the list of notifiers.
    */
    void RemoveNotifier(wxDataViewModelNotifier* notifier);

    /**
        Call this to initiate a resort after the sort function has been changed.
    */
    virtual void Resort();

    /**
        This gets called in order to set a value in the data model.

        The most common scenario is that the wxDataViewCtrl calls this method
        after the user changed some data in the view.

        This is the function you need to override in your derived class but if
        you want to call it, ChangeValue() is usually more convenient as
        otherwise you need to manually call ValueChanged() to update the
        control itself.
    */
    virtual bool SetValue(const wxVariant& variant,
                          const wxDataViewItem& item,
                          unsigned int col) = 0;

    /**
        Call this to inform this model that a value in the model has been changed.
        This is also called from wxDataViewCtrl's internal editing code, e.g. when
        editing a text field in the control.

        This will eventually emit a @c wxEVT_DATAVIEW_ITEM_VALUE_CHANGED
        event to the user.
    */
    virtual bool ValueChanged(const wxDataViewItem& item,
                              unsigned int col);

protected:

    /**
        Destructor. This should not be called directly. Use DecRef() instead.
    */
    virtual ~wxDataViewModel();
};



/**
    @class wxDataViewListModel

    Base class with abstract API for wxDataViewIndexListModel and
    wxDataViewVirtualListModel.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewListModel : public wxDataViewModel
{
public:

    /**
        Destructor.
    */
    virtual ~wxDataViewIndexListModel();

    /**
        Compare method that sorts the items by their index.
    */
    int Compare(const wxDataViewItem& item1,
                const wxDataViewItem& item2,
                unsigned int column, bool ascending) const;

    /**
        Override this to indicate that the row has special font attributes.
        This only affects the wxDataViewTextRendererText() renderer.

        The base class version always simply returns @false.

        @see wxDataViewItemAttr.

        @param row
            The row for which the attribute is requested.
        @param col
            The column for which the attribute is requested.
        @param attr
            The attribute to be filled in if the function returns @true.
        @return
            @true if this item has an attribute or @false otherwise.
    */
    virtual bool GetAttrByRow(unsigned int row, unsigned int col,
                         wxDataViewItemAttr& attr) const;

    /**
        Override this if you want to disable specific items.

        The base class version always returns @true, thus making all items
        enabled by default.

        @param row
            The row of the item whose enabled status is requested.
        @param col
            The column of the item whose enabled status is requested.
        @return
            @true if the item at this row and column should be enabled,
            @false otherwise.

        @note See wxDataViewModel::IsEnabled() for the current status of
              support for disabling the items under different platforms.

        @since 2.9.2
    */
    virtual bool IsEnabledByRow(unsigned int row,
                                unsigned int col) const;

    /**
        Returns the number of items (or rows) in the list.
    */
    unsigned int GetCount() const;

    /**
        Returns the wxDataViewItem at the given @e row.
    */
    wxDataViewItem GetItem(unsigned int row) const;

    /**
        Returns the position of given @e item.
    */
    unsigned int GetRow(const wxDataViewItem& item) const;

    /**
        Override this to allow getting values from the model.
    */
    virtual void GetValueByRow(wxVariant& variant, unsigned int row,
                          unsigned int col) const = 0;

    /**
        Call this after if the data has to be read again from the model.
        This is useful after major changes when calling the methods below
        (possibly thousands of times) doesn't make sense.
    */
    void Reset(unsigned int new_size);

    /**
        Call this after a row has been appended to the model.
    */
    void RowAppended();

    /**
        Call this after a row has been changed.
    */
    void RowChanged(unsigned int row);

    /**
        Call this after a row has been deleted.
    */
    void RowDeleted(unsigned int row);

    /**
        Call this after a row has been inserted at the given position.
    */
    void RowInserted(unsigned int before);

    /**
        Call this after a row has been prepended to the model.
    */
    void RowPrepended();

    /**
        Call this after a value has been changed.
    */
    void RowValueChanged(unsigned int row, unsigned int col);

    /**
        Call this after rows have been deleted.
        The array will internally get copied and sorted in descending order so
        that the rows with the highest position will be deleted first.
    */
    void RowsDeleted(const wxArrayInt& rows);

    /**
        Called in order to set a value in the model.
    */
    virtual bool SetValueByRow(const wxVariant& variant, unsigned int row,
                          unsigned int col) = 0;
};


/**
    @class wxDataViewIndexListModel

    wxDataViewIndexListModel is a specialized data model which lets you address
    an item by its position (row) rather than its wxDataViewItem (which you can
    obtain from this class).
    This model also provides its own wxDataViewIndexListModel::Compare
    method which sorts the model's data by the index.

    This model is not a virtual model since the control stores each wxDataViewItem.
    Use wxDataViewVirtualListModel if you need to display millions of items or
    have other reason to use a virtual control.

    @see wxDataViewListModel for the API.
    
    @library{wxadv}
    @category{dvc}
*/

class wxDataViewIndexListModel : public wxDataViewListModel
{
public:
    /**
        Constructor.
    */
    wxDataViewIndexListModel(unsigned int initial_size = 0);

};

/**
    @class wxDataViewVirtualListModel

    wxDataViewVirtualListModel is a specialized data model which lets you address
    an item by its position (row) rather than its wxDataViewItem and as such offers
    the exact same interface as wxDataViewIndexListModel.
    The important difference is that under platforms other than OS X, using this
    model will result in a truly virtual control able to handle millions of items
    as the control doesn't store any item (a feature not supported by OS X).

    @see wxDataViewListModel for the API.

    @library{wxadv}
    @category{dvc}
*/

class wxDataViewVirtualListModel : public wxDataViewListModel
{
public:
    /**
        Constructor.
    */
    wxDataViewVirtualListModel(unsigned int initial_size = 0);

};



/**
    @class wxDataViewItemAttr

    This class is used to indicate to a wxDataViewCtrl that a certain item
    (see wxDataViewItem) has extra font attributes for its renderer.
    For this, it is required to override wxDataViewModel::GetAttr.

    Attributes are currently only supported by wxDataViewTextRendererText.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewItemAttr
{
public:
    /**
        Constructor.
    */
    wxDataViewItemAttr();

    /**
        Call this to indicate that the item shall be displayed in bold text.
    */
    void SetBold(bool set);

    /**
        Call this to indicate that the item shall be displayed with that colour.
    */
    void SetColour(const wxColour& colour);

    /**
        Call this to set the background colour to use.

        Currently this attribute is only supported in the generic version of
        wxDataViewCtrl and ignored by the native GTK+ and OS X implementations.

        @since 2.9.4
    */
    void SetBackgroundColour(const wxColour& colour);

    /**
        Call this to indicate that the item shall be displayed in italic text.
    */
    void SetItalic(bool set);
};



/**
    @class wxDataViewItem

    wxDataViewItem is a small opaque class that represents an item in a wxDataViewCtrl
    in a persistent way, i.e. independent of the position of the item in the control
    or changes to its contents.

    It must hold a unique ID of type @e void* in its only field and can be converted
    to and from it.

    If the ID is @NULL the wxDataViewItem is invalid and wxDataViewItem::IsOk will
    return @false which used in many places in the API of wxDataViewCtrl to
    indicate that e.g. no item was found. An ID of @NULL is also used to indicate
    the invisible root. Examples for this are wxDataViewModel::GetParent and
    wxDataViewModel::GetChildren.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewItem
{
public:
    //@{
    /**
        Constructor.
    */
    wxDataViewItem();
    wxDataViewItem(const wxDataViewItem& item);
    explicit wxDataViewItem(void* id);
    //@}

    /**
        Returns the ID.
    */
    void* GetID() const;

    /**
        Returns @true if the ID is not @NULL.
    */
    bool IsOk() const;
};


// ----------------------------------------------------------------------------
// wxDataViewCtrl flags
// ----------------------------------------------------------------------------

// size of a wxDataViewRenderer without contents:
#define wxDVC_DEFAULT_RENDERER_SIZE     20

// the default width of new (text) columns:
#define wxDVC_DEFAULT_WIDTH             80

// the default width of new toggle columns:
#define wxDVC_TOGGLE_DEFAULT_WIDTH      30

// the default minimal width of the columns:
#define wxDVC_DEFAULT_MINWIDTH          30

// The default alignment of wxDataViewRenderers is to take
// the alignment from the column it owns.
#define wxDVR_DEFAULT_ALIGNMENT         -1

#define wxDV_SINGLE                  0x0000     // for convenience
#define wxDV_MULTIPLE                0x0001     // can select multiple items

#define wxDV_NO_HEADER               0x0002     // column titles not visible
#define wxDV_HORIZ_RULES             0x0004     // light horizontal rules between rows
#define wxDV_VERT_RULES              0x0008     // light vertical rules between columns

#define wxDV_ROW_LINES               0x0010     // alternating colour in rows
#define wxDV_VARIABLE_LINE_HEIGHT    0x0020     // variable line height

// events

wxEventType wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED;

wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED;

wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU;

wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK;
wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED;
wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED;
wxEventType wxEVT_COMMAND_DATAVIEW_CACHE_HINT;

wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE;
wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_DROP;

/**
    @class wxDataViewCtrl

    wxDataViewCtrl is a control to display data either in a tree like fashion or
    in a tabular form or both.

    If you only need to display a simple tree structure with an API more like the
    older wxTreeCtrl class, then the specialized wxDataViewTreeCtrl can be used.
    Likewise, if you only want to display simple table structure you can use
    the specialized wxDataViewListCtrl class. Both wxDataViewTreeCtrl and
    wxDataViewListCtrl can be used without defining  your own wxDataViewModel.

    A wxDataViewItem is used to represent a (visible) item in the control.

    Unlike wxListCtrl, wxDataViewCtrl doesn't get its data from the user through
    virtual functions or by setting it directly. Instead you need to write your own
    wxDataViewModel and associate it with this control.
    Then you need to add a number of wxDataViewColumn to this control to define
    what each column shall display. Each wxDataViewColumn in turn owns 1 instance
    of a wxDataViewRenderer to render its cells.

    A number of standard renderers for rendering text, dates, images, toggle,
    a progress bar etc. are provided. Additionally, the user can write custom
    renderers deriving from wxDataViewCustomRenderer for displaying anything.

    All data transfer from the control to the model and the user code is done
    through wxVariant which can be extended to support more data formats as necessary.
    Accordingly, all type information uses the strings returned from wxVariant::GetType.

    @beginStyleTable
    @style{wxDV_SINGLE}
           Single selection mode. This is the default.
    @style{wxDV_MULTIPLE}
           Multiple selection mode.
    @style{wxDV_ROW_LINES}
           Use alternating colours for rows if supported by platform and theme.
           Currently only supported by the native GTK and OS X implementations
           but not by the generic one.
    @style{wxDV_HORIZ_RULES}
           Display fine rules between row if supported.
    @style{wxDV_VERT_RULES}
           Display fine rules between columns is supported.
    @style{wxDV_VARIABLE_LINE_HEIGHT}
           Allow variable line heights.
           This can be inefficient when displaying large number of items.
    @style{wxDV_NO_HEADER}
           Do not show column headers (which are shown by default).
    @endStyleTable

    @beginEventEmissionTable{wxDataViewEvent}
    @event{EVT_DATAVIEW_SELECTION_CHANGED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED event.
    @event{EVT_DATAVIEW_ITEM_ACTIVATED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event. This event
           is triggered by double clicking an item or pressing some special key
           (usually "Enter") when it is focused.
    @event{EVT_DATAVIEW_ITEM_START_EDITING(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_START_EDITING event. This
           event can be vetoed in order to prevent editing on an item by item
           basis.
    @event{EVT_DATAVIEW_ITEM_EDITING_STARTED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED event.
    @event{EVT_DATAVIEW_ITEM_EDITING_DONE(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE event.
    @event{EVT_DATAVIEW_ITEM_COLLAPSING(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING event.
    @event{EVT_DATAVIEW_ITEM_COLLAPSED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED event.
    @event{EVT_DATAVIEW_ITEM_EXPANDING(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING event.
    @event{EVT_DATAVIEW_ITEM_EXPANDED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED event.
    @event{EVT_DATAVIEW_ITEM_VALUE_CHANGED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED event.
    @event{EVT_DATAVIEW_ITEM_CONTEXT_MENU(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU event
           generated when the user right clicks inside the control. Notice that
           this menu is generated even if the click didn't occur on any valid
           item, in this case wxDataViewEvent::GetItem() simply returns an
           invalid item.
    @event{EVT_DATAVIEW_COLUMN_HEADER_CLICK(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK event.
    @event{EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED event.
    @event{EVT_DATAVIEW_COLUMN_SORTED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED event.
    @event{EVT_DATAVIEW_COLUMN_REORDERED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED event.
    @event{EVT_DATAVIEW_ITEM_BEGIN_DRAG(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG event.
    @event{EVT_DATAVIEW_ITEM_DROP_POSSIBLE(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE event.
    @event{EVT_DATAVIEW_ITEM_DROP(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_DROP event.
    @endEventTable

    Notice that this control doesn't allow to process generic mouse events such
    as @c wxEVT_LEFT_DOWN in all ports (notably it doesn't work in wxGTK). If
    you need to handle any mouse events not covered by the ones above, consider
    using a custom renderer for the cells that must handle them.

    @library{wxadv}
    @category{ctrl,dvc}
    @appearance{dataviewctrl.png}
*/
class wxDataViewCtrl : public wxControl
{
public:
    /**
        Default Constructor.
    */
    wxDataViewCtrl();

    /**
        Constructor. Calls Create().
    */
    wxDataViewCtrl(wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxDataViewCtrlNameStr);

    /**
        Destructor.
    */
    virtual ~wxDataViewCtrl();

    /**
        Appends a wxDataViewColumn to the control. Returns @true on success.

        Note that there is a number of short cut methods which implicitly create
        a wxDataViewColumn and a wxDataViewRenderer for it (see below).
    */
    virtual bool AppendColumn(wxDataViewColumn* col);

    /**
        Prepends a wxDataViewColumn to the control. Returns @true on success.

        Note that there is a number of short cut methods which implicitly create
        a wxDataViewColumn and a wxDataViewRenderer for it.
    */
    virtual bool PrependColumn(wxDataViewColumn* col);

    /**
        Inserts a wxDataViewColumn to the control. Returns @true on success.
    */
    virtual bool InsertColumn(unsigned int pos, wxDataViewColumn* col);

    //@{
    /**
        Appends a column for rendering a bitmap. Returns the wxDataViewColumn
        created in the function or @NULL on failure.
    */
    wxDataViewColumn* AppendBitmapColumn(const wxString& label,
                                         unsigned int model_column,
                                         wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                         int width = -1,
                                         wxAlignment align = wxALIGN_CENTER,
                                         int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendBitmapColumn(const wxBitmap& label,
                                         unsigned int model_column,
                                         wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                         int width = -1,
                                         wxAlignment align = wxALIGN_CENTER,
                                         int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    //@{
    /**
        Appends a column for rendering a date. Returns the wxDataViewColumn
        created in the function or @NULL on failure.

        @note The @a align parameter is applied to both the column header and
              the column renderer.
    */
    wxDataViewColumn* AppendDateColumn(const wxString& label,
                                       unsigned int model_column,
                                       wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                                       int width = -1,
                                       wxAlignment align = wxALIGN_NOT,
                                       int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendDateColumn(const wxBitmap& label,
                                       unsigned int model_column,
                                       wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                                       int width = -1,
                                       wxAlignment align = wxALIGN_NOT,
                                       int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    //@{
    /**
        Appends a column for rendering text with an icon. Returns the wxDataViewColumn
        created in the function or @NULL on failure.
        This method uses the wxDataViewIconTextRenderer class.

        @note The @a align parameter is applied to both the column header and
              the column renderer.
    */
    wxDataViewColumn* AppendIconTextColumn(const wxString& label,
                                           unsigned int model_column,
                                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = -1,
                                           wxAlignment align = wxALIGN_NOT,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendIconTextColumn(const wxBitmap& label,
                                           unsigned int model_column,
                                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = -1,
                                           wxAlignment align = wxALIGN_NOT,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    //@{
    /**
        Appends a column for rendering a progress indicator. Returns the
        wxDataViewColumn created in the function or @NULL on failure.

        @note The @a align parameter is applied to both the column header and
              the column renderer.
    */
    wxDataViewColumn* AppendProgressColumn(const wxString& label,
                                           unsigned int model_column,
                                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = 80,
                                           wxAlignment align = wxALIGN_CENTER,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendProgressColumn(const wxBitmap& label,
                                           unsigned int model_column,
                                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                           int width = 80,
                                           wxAlignment align = wxALIGN_CENTER,
                                           int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    //@{
    /**
        Appends a column for rendering text. Returns the wxDataViewColumn
        created in the function or @NULL on failure.

        @note The @a align parameter is applied to both the column header and
              the column renderer.
    */
    wxDataViewColumn* AppendTextColumn(const wxString& label,
                                       unsigned int model_column,
                                       wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                       int width = -1,
                                       wxAlignment align = wxALIGN_NOT,
                                       int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendTextColumn(const wxBitmap& label,
                                       unsigned int model_column,
                                       wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                       int width = -1,
                                       wxAlignment align = wxALIGN_NOT,
                                       int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    //@{
    /**
        Appends a column for rendering a toggle. Returns the wxDataViewColumn
        created in the function or @NULL on failure.

        @note The @a align parameter is applied to both the column header and
              the column renderer.
    */
    wxDataViewColumn* AppendToggleColumn(const wxString& label,
                                         unsigned int model_column,
                                         wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                         int width = 30,
                                         wxAlignment align = wxALIGN_CENTER,
                                         int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn* AppendToggleColumn(const wxBitmap& label,
                                         unsigned int model_column,
                                         wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                         int width = 30,
                                         wxAlignment align = wxALIGN_CENTER,
                                         int flags = wxDATAVIEW_COL_RESIZABLE);
    //@}

    /**
        Associates a wxDataViewModel with the control.
        This increases the reference count of the model by 1.
    */
    virtual bool AssociateModel(wxDataViewModel* model);

    /**
        Removes all columns.
    */
    virtual bool ClearColumns();

    /**
        Collapses the item.
    */
    virtual void Collapse(const wxDataViewItem& item);

    /**
        Create the control. Useful for two step creation.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDataViewCtrlNameStr);

    /**
        Deletes given column.
    */
    virtual bool DeleteColumn(wxDataViewColumn* column);

    /**
        Programmatically starts editing given cell of @a item.

        Doesn't do anything if the item or this column is not editable.

        @note Currently not implemented in wxOSX/Carbon.

        @since 2.9.4
    */
    virtual void EditItem(const wxDataViewItem& item, const wxDataViewColumn *column);

    /**
       Enable drag operations using the given @a format.
    */
    virtual bool EnableDragSource( const wxDataFormat &format );

    /**
       Enable drop operations using the given @a format.
    */
    virtual bool EnableDropTarget( const wxDataFormat &format );

    /**
        Call this to ensure that the given item is visible.
    */
    virtual void EnsureVisible(const wxDataViewItem& item,
                               const wxDataViewColumn* column = NULL);

    /**
        Expands the item.
    */
    virtual void Expand(const wxDataViewItem& item);

    /**
       Expands all ancestors of the @a item. This method also
       ensures that the item itself as well as all ancestor
       items have been read from the model by the control.
    */
    virtual void ExpandAncestors( const wxDataViewItem & item );

    /**
        Returns pointer to the column. @a pos refers to the position in the
        control which may change after reordering columns by the user.
    */
    virtual wxDataViewColumn* GetColumn(unsigned int pos) const;

    /**
        Returns the number of columns.
    */
    virtual unsigned int GetColumnCount() const;

    /**
        Returns the position of the column or -1 if not found in the control.
    */
    virtual int GetColumnPosition(const wxDataViewColumn* column) const;

    /**
        Returns column containing the expanders.
    */
    wxDataViewColumn* GetExpanderColumn() const;

    /**
        Returns the currently focused item.

        This is the item that the keyboard commands apply to. It may be invalid
        if there is no focus currently.

        This method is mostly useful for the controls with @c wxDV_MULTIPLE
        style as in the case of single selection it returns the same thing as
        GetSelection().

        Notice that under all platforms except Mac OS X the currently focused
        item may be selected or not but under OS X the current item is always
        selected.

        @see SetCurrentItem(), GetCurrentColumn()

        @since 2.9.2
     */
    wxDataViewItem GetCurrentItem() const;

    /**
        Returns the column that currently has focus.

        If the focus is set to individual cell within the currently focused
        item (as opposed to being on the item as a whole), then this is the
        column that the focus is on.

        Returns NULL if no column currently has focus.

        @see GetCurrentItem()

        @since 2.9.4
     */
    wxDataViewColumn *GetCurrentColumn() const;

    /**
        Returns indentation.
    */
    int GetIndent() const;

    /**
        Returns item rectangle.

        This method is currently not implemented at all in wxGTK and only
        implemented for non-@NULL @a col argument in wxOSX. It is fully
        implemented in the generic version of the control.

        @param item
            A valid item.
        @param col
            If non-@NULL, the rectangle returned corresponds to the
            intersection of the item with the specified column. If @NULL, the
            rectangle spans all the columns.
    */
    virtual wxRect GetItemRect(const wxDataViewItem& item,
                               const wxDataViewColumn* col = NULL) const;

    /**
        Returns pointer to the data model associated with the control (if any).
    */
    wxDataViewModel* GetModel();

    /**
        Returns the number of currently selected items.

        This method may be called for both the controls with single and
        multiple selections and returns the number of selected item, possibly
        0, in any case.

        @since 2.9.3
     */
    virtual int GetSelectedItemsCount() const;

    /**
        Returns first selected item or an invalid item if none is selected.

        This method may be called for both the controls with single and
        multiple selections but returns an invalid item if more than one item
        is selected in the latter case, use HasSelection() to determine if
        there are any selected items when using multiple selection.
    */
    virtual wxDataViewItem GetSelection() const;

    /**
        Fills @a sel with currently selected items and returns their number.

        This method may be called for both the controls with single and
        multiple selections. In the single selection case it returns the array
        with at most one element in it.

        @see GetSelectedItemsCount()
    */
    virtual int GetSelections(wxDataViewItemArray& sel) const;

    /**
        Returns the wxDataViewColumn currently responsible for sorting
        or @NULL if none has been selected.
    */
    virtual wxDataViewColumn* GetSortingColumn() const;

    /**
        Returns true if any items are currently selected.

        This method may be called for both the controls with single and
        multiple selections.

        Calling this method is equivalent to calling GetSelectedItemsCount()
        and comparing its result with 0 but is more clear and might also be
        implemented more efficiently in the future.

        @since 2.9.3
     */
    bool HasSelection() const;

    /**
        Hittest.
    */
    virtual void HitTest(const wxPoint& point, wxDataViewItem& item,
                         wxDataViewColumn*& col) const;

    /**
        Return @true if the item is expanded.
    */
    virtual bool IsExpanded(const wxDataViewItem& item) const;

    /**
        Return @true if the item is selected.
    */
    virtual bool IsSelected(const wxDataViewItem& item) const;

    /**
        Select the given item.

        In single selection mode this changes the (unique) currently selected
        item. In multi selection mode, the @a item is selected and the
        previously selected items remain selected.
    */
    virtual void Select(const wxDataViewItem& item);

    /**
        Select all items.
    */
    virtual void SelectAll();

    /**
        Set which column shall contain the tree-like expanders.
    */
    void SetExpanderColumn(wxDataViewColumn* col);

    /**
        Changes the currently focused item.

        The @a item parameter must be valid, there is no way to remove the
        current item from the control.

        In single selection mode, calling this method is the same as calling
        Select() and is thus not very useful. In multiple selection mode this
        method only moves the current item however without changing the
        selection except under OS X where the current item is always selected,
        so calling SetCurrentItem() selects @a item if it hadn't been selected
        before.

        @see GetCurrentItem()

        @since 2.9.2
     */
    void SetCurrentItem(const wxDataViewItem& item);

    /**
        Sets the indentation.
    */
    void SetIndent(int indent);

    /**
        Sets the selection to the array of wxDataViewItems.
    */
    virtual void SetSelections(const wxDataViewItemArray& sel);

    /**
        Unselect the given item.
    */
    virtual void Unselect(const wxDataViewItem& item);

    /**
        Unselect all item.
        This method only has effect if multiple selections are allowed.
    */
    virtual void UnselectAll();

    /**
        Sets the row height.

        This function can only be used when all rows have the same height, i.e.
        when wxDV_VARIABLE_LINE_HEIGHT flag is not used.

        Currently this is implemented in the generic and native GTK versions
        only and nothing is done (and @false returned) when using OS X port.

        Also notice that this method can only be used to increase the row
        height compared with the default one (as determined by the return value
        of wxDataViewRenderer::GetSize()), if it is set to a too small value
        then the minimum required by the renderers will be used.

        @return @true if the line height was changed or @false otherwise.

        @since 2.9.2
    */
    virtual bool SetRowHeight(int rowHeight);
};



/**
    @class wxDataViewModelNotifier

    A wxDataViewModelNotifier instance is owned by a wxDataViewModel and mirrors
    its notification interface.
    See the documentation of that class for further information.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewModelNotifier
{
public:
    /**
        Constructor.
    */
    wxDataViewModelNotifier();

    /**
        Destructor.
    */
    virtual ~wxDataViewModelNotifier();

    /**
        Called by owning model.
    */
    virtual bool Cleared() = 0;

    /**
        Get owning wxDataViewModel.
    */
    wxDataViewModel* GetOwner() const;

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemAdded(const wxDataViewItem& parent,
                           const wxDataViewItem& item) = 0;

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemChanged(const wxDataViewItem& item) = 0;

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemDeleted(const wxDataViewItem& parent,
                             const wxDataViewItem& item) = 0;

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemsAdded(const wxDataViewItem& parent,
                            const wxDataViewItemArray& items);

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemsChanged(const wxDataViewItemArray& items);

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ItemsDeleted(const wxDataViewItem& parent,
                              const wxDataViewItemArray& items);

    /**
        Called by owning model.
    */
    virtual void Resort() = 0;

    /**
        Set owner of this notifier. Used internally.
    */
    void SetOwner(wxDataViewModel* owner);

    /**
        Called by owning model.

        @return Always return @true from this function in derived classes.
    */
    virtual bool ValueChanged(const wxDataViewItem& item, unsigned int col) = 0;
};


/**
    The mode of a data-view cell; see wxDataViewRenderer for more info.
*/
enum wxDataViewCellMode
{
    /**
        The cell only displays information and cannot be manipulated or
        otherwise interacted with in any way.

        Note that this doesn't mean that the row being drawn can't be selected,
        just that a particular element of it cannot be individually modified.
     */
    wxDATAVIEW_CELL_INERT,

    /**
        Indicates that the cell can be @em activated by clicking it or using
        keyboard.

        Activating a cell is an alternative to showing inline editor when the
        value can be edited in a simple way that doesn't warrant full editor
        control. The most typical use of cell activation is toggling the
        checkbox in wxDataViewToggleRenderer; others would be e.g. an embedded
        volume slider or a five-star rating column.

        The exact means of activating a cell are platform-dependent, but they
        are usually similar to those used for inline editing of values.
        Typically, a cell would be activated by Space or Enter keys or by left
        mouse click.

        @note Do not confuse this with item activation in wxDataViewCtrl
              and the wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event. That one is
              used for activating the item (or, to put it differently, the
              entire row) similarly to analogous messages in wxTreeCtrl and
              wxListCtrl, and the effect differs (play a song, open a file
              etc.). Cell activation, on the other hand, is all about
              interacting with the individual cell.

        @see wxDataViewCustomRenderer::ActivateCell()
    */
    wxDATAVIEW_CELL_ACTIVATABLE,

    /**
        Indicates that the user can edit the data in-place in an inline editor
        control that will show up when the user wants to edit the cell.

        A typical example of this behaviour is changing the filename in a file
        managers.

        Editing is typically triggered by slowly double-clicking the cell or by
        a platform-dependent keyboard shortcut (F2 is typical on Windows, Space
        and/or Enter is common elsewhere and supported on Windows too).

        @see wxDataViewCustomRenderer::CreateEditorCtrl()
    */
    wxDATAVIEW_CELL_EDITABLE
};

/**
    The values of this enum controls how a wxDataViewRenderer should display
    its contents in a cell.
*/
enum wxDataViewCellRenderState
{
    wxDATAVIEW_CELL_SELECTED    = 1,
    wxDATAVIEW_CELL_PRELIT      = 2,
    wxDATAVIEW_CELL_INSENSITIVE = 4,
    wxDATAVIEW_CELL_FOCUSED     = 8
};

/**
    @class wxDataViewRenderer

    This class is used by wxDataViewCtrl to render the individual cells.
    One instance of a renderer class is owned by a wxDataViewColumn.
    There is a number of ready-to-use renderers provided:
    - wxDataViewTextRenderer,
    - wxDataViewIconTextRenderer,
    - wxDataViewToggleRenderer,
    - wxDataViewProgressRenderer,
    - wxDataViewBitmapRenderer,
    - wxDataViewDateRenderer,
    - wxDataViewSpinRenderer.
    - wxDataViewChoiceRenderer.

    Additionally, the user can write own renderers by deriving from
    wxDataViewCustomRenderer.

    The ::wxDataViewCellMode and ::wxDataViewCellRenderState flags accepted
    by the constructors respectively controls what actions the cell data allows
    and how the renderer should display its contents in a cell.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewRenderer : public wxObject
{
public:
    /**
        Constructor.
    */
    wxDataViewRenderer(const wxString& varianttype,
                       wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                       int align = wxDVR_DEFAULT_ALIGNMENT );

    /**
        Enable or disable replacing parts of the item text with ellipsis to
        make it fit the column width.

        This method only makes sense for the renderers working with text, such
        as wxDataViewTextRenderer or wxDataViewIconTextRenderer.

        By default wxELLIPSIZE_MIDDLE is used.

        @param mode
            Ellipsization mode, use wxELLIPSIZE_NONE to disable.

        @since 2.9.1
     */
    void EnableEllipsize(wxEllipsizeMode mode = wxELLIPSIZE_MIDDLE);

    /**
        Disable replacing parts of the item text with ellipsis.

        If ellipsizing is disabled, the string will be truncated if it doesn't
        fit.

        This is the same as @code EnableEllipsize(wxELLIPSIZE_NONE) @endcode.

        @since 2.9.1
     */
    void DisableEllipsize();

    /**
        Returns the alignment. See SetAlignment()
    */
    virtual int GetAlignment() const;

    /**
        Returns the ellipsize mode used by the renderer.

        If the return value is wxELLIPSIZE_NONE, the text is simply truncated
        if it doesn't fit.

        @see EnableEllipsize()
     */
    wxEllipsizeMode GetEllipsizeMode() const;

    /**
        Returns the cell mode.
    */
    virtual wxDataViewCellMode GetMode() const;

    /**
        Returns pointer to the owning wxDataViewColumn.
    */
    wxDataViewColumn* GetOwner() const;

    /**
        This methods retrieves the value from the renderer in order to
        transfer the value back to the data model.

        Returns @false on failure.
    */
    virtual bool GetValue(wxVariant& value) const = 0;

    /**
        Returns a string with the type of the wxVariant supported by this renderer.
    */
    wxString GetVariantType() const;

    /**
        Sets the alignment of the renderer's content.
        The default value of @c wxDVR_DEFAULT_ALIGMENT indicates that the content
        should have the same alignment as the column header.

        The method is not implemented under OS X and the renderer always aligns
        its contents as the column header on that platform. The other platforms
        support both vertical and horizontal alignment.
    */
    virtual void SetAlignment( int align );
    /**
        Sets the owning wxDataViewColumn.
        This is usually called from within wxDataViewColumn.
    */
    void SetOwner(wxDataViewColumn* owner);

    /**
        Set the value of the renderer (and thus its cell) to @a value.
        The internal code will then render this cell with this data.
    */
    virtual bool SetValue(const wxVariant& value) = 0;

    /**
        Before data is committed to the data model, it is passed to this
        method where it can be checked for validity. This can also be
        used for checking a valid range or limiting the user input in
        a certain aspect (e.g. max number of characters or only alphanumeric
        input, ASCII only etc.). Return @false if the value is not valid.

        Please note that due to implementation limitations, this validation
        is done after the editing control already is destroyed and the
        editing process finished.
    */
    virtual bool Validate(wxVariant& value);
};



/**
    @class wxDataViewTextRenderer

    wxDataViewTextRenderer is used for rendering text.
    It supports in-place editing if desired.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewTextRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewTextRenderer(const wxString& varianttype = "string",
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                           int align = wxDVR_DEFAULT_ALIGNMENT );
};



/**
    @class wxDataViewIconTextRenderer

    The wxDataViewIconTextRenderer class is used to display text with
    a small icon next to it as it is typically done in a file manager.

    This classes uses the wxDataViewIconText helper class to store its data.
    wxDataViewIconText can be converted to and from a wxVariant using the left
    shift operator.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewIconTextRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewIconTextRenderer(const wxString& varianttype = "wxDataViewIconText",
                               wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                               int align = wxDVR_DEFAULT_ALIGNMENT );
};



/**
    @class wxDataViewProgressRenderer

    This class is used by wxDataViewCtrl to render progress bars.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewProgressRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewProgressRenderer(const wxString& label = wxEmptyString,
                               const wxString& varianttype = "long",
                               wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                               int align = wxDVR_DEFAULT_ALIGNMENT );
};



/**
    @class wxDataViewSpinRenderer

    This is a specialized renderer for rendering integer values.
    It supports modifying the values in-place by using a wxSpinCtrl.
    The renderer only support variants of type @e long.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewSpinRenderer : public wxDataViewCustomRenderer
{
public:
    /**
        Constructor.
        @a min and @a max indicate the minimum and maximum values for the wxSpinCtrl.
    */
    wxDataViewSpinRenderer(int min, int max,
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                           int align = wxDVR_DEFAULT_ALIGNMENT);
};



/**
    @class wxDataViewToggleRenderer

    This class is used by wxDataViewCtrl to render toggle controls.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewToggleRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewToggleRenderer(const wxString& varianttype = "bool",
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = wxDVR_DEFAULT_ALIGNMENT);
};


/**
    A wxDataViewCtrl renderer using wxChoice control and values of strings in
    it.

    This class is used by wxDataViewCtrl to render choice controls.
    It stores a string so that SetValue() and GetValue() operate
    on a variant holding a string.

    @see wxDataViewChoiceByIndexRenderer

    @library{wxadv}
    @category{dvc}
*/

class wxDataViewChoiceRenderer: public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewChoiceRenderer( const wxArrayString &choices,
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                              int alignment = wxDVR_DEFAULT_ALIGNMENT );

    /**
        Returns the choice referred to by index.
    */
    wxString GetChoice(size_t index) const;

    /**
        Returns all choices.
    */
    const wxArrayString& GetChoices() const;
};


/**
    A wxDataViewCtrl renderer using wxChoice control and indexes into it.

    Unlike its base wxDataViewChoiceRenderer class, this one stores the choice
    index, i.e. an @c int, in the variant used by its SetValue() and
    GetValue().

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewChoiceByIndexRenderer : public wxDataViewChoiceRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewChoiceByIndexRenderer( const wxArrayString &choices,
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                              int alignment = wxDVR_DEFAULT_ALIGNMENT );
};


/**
    @class wxDataViewDateRenderer

    This class is used by wxDataViewCtrl to render calendar controls.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewDateRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewDateRenderer(const wxString& varianttype = "datetime",
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                           int align = wxDVR_DEFAULT_ALIGNMENT);
};



/**
    @class wxDataViewCustomRenderer

    You need to derive a new class from wxDataViewCustomRenderer in
    order to write a new renderer.

    You need to override at least wxDataViewRenderer::SetValue, wxDataViewRenderer::GetValue,
    wxDataViewCustomRenderer::GetSize and wxDataViewCustomRenderer::Render.

    If you want your renderer to support in-place editing then you also need to override
    wxDataViewCustomRenderer::HasEditorCtrl, wxDataViewCustomRenderer::CreateEditorCtrl
    and wxDataViewCustomRenderer::GetValueFromEditorCtrl.

    Note that a special event handler will be pushed onto that editor control
    which handles @e \<ENTER\> and focus out events in order to end the editing.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewCustomRenderer : public wxDataViewRenderer
{
public:
    /**
        Constructor.
    */
    wxDataViewCustomRenderer(const wxString& varianttype = "string",
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = -1, bool no_init = false);

    /**
        Destructor.
    */
    virtual ~wxDataViewCustomRenderer();

    /**
        Override this to react to cell @em activation. Activating a cell is an
        alternative to showing inline editor when the value can be edited in a
        simple way that doesn't warrant full editor control. The most typical
        use of cell activation is toggling the checkbox in
        wxDataViewToggleRenderer; others would be e.g. an embedded volume
        slider or a five-star rating column.

        The exact means of activating a cell are platform-dependent, but they
        are usually similar to those used for inline editing of values.
        Typically, a cell would be activated by Space or Enter keys or by left
        mouse click.

        This method will only be called if the cell has the
        wxDATAVIEW_CELL_ACTIVATABLE mode.

        @param cell
            Coordinates of the activated cell's area.
        @param model
            The model to manipulate in response.
        @param item
            Activated item.
        @param col
            Activated column of @a item.
        @param mouseEvent
            If the activation was triggered by mouse click, contains the
            corresponding event. Is @NULL otherwise (for keyboard activation).
            Mouse coordinates are adjusted to be relative to the cell.

        @since 2.9.3

        @note Do not confuse this method with item activation in wxDataViewCtrl
              and the wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event. That one is
              used for activating the item (or, to put it differently, the
              entire row) similarly to analogous messages in wxTreeCtrl and
              wxListCtrl, and the effect differs (play a song, open a file
              etc.). Cell activation, on the other hand, is all about
              interacting with the individual cell.

        @see CreateEditorCtrl()
    */
    virtual bool ActivateCell(const wxRect& cell,
                              wxDataViewModel* model,
                              const wxDataViewItem & item,
                              unsigned int col,
                              const wxMouseEvent *mouseEvent);

    /**
        Override this to create the actual editor control once editing
        is about to start.

        This method will only be called if the cell has the
        wxDATAVIEW_CELL_EDITABLE mode. Editing is typically triggered by slowly
        double-clicking the cell or by a platform-dependent keyboard shortcut
        (F2 is typical on Windows, Space and/or Enter is common elsewhere and
        supported on Windows too).

        @param parent
            The parent of the editor control.
        @param labelRect
            Indicates the position and size of the editor control. The control
            should be created in place of the cell and @a labelRect should be
            respected as much as possible.
        @param value
            Initial value of the editor.

        An example:
        @code
        {
            long l = value;
            return new wxSpinCtrl( parent, wxID_ANY, wxEmptyString,
                        labelRect.GetTopLeft(), labelRect.GetSize(), 0, 0, 100, l );
        }
        @endcode

        @see ActivateCell()
    */
    virtual wxWindow* CreateEditorCtrl(wxWindow* parent,
                                       wxRect labelRect,
                                       const wxVariant& value);

    /**
        Return the attribute to be used for rendering.

        This function may be called from Render() implementation to use the
        attributes defined for the item if the renderer supports them.

        Notice that when Render() is called, the wxDC object passed to it is
        already set up to use the correct attributes (e.g. its font is set to
        bold or italic version if wxDataViewItemAttr::GetBold() or GetItalic()
        returns true) so it may not be necessary to call it explicitly if you
        only want to render text using the items attributes.

        @since 2.9.1
     */
    const wxDataViewItemAttr& GetAttr() const;

    /**
        Return size required to show content.
    */
    virtual wxSize GetSize() const = 0;

    /**
        Override this so that the renderer can get the value from the editor
        control (pointed to by @a editor):
        @code
        {
            wxSpinCtrl *sc = (wxSpinCtrl*) editor;
            long l = sc->GetValue();
            value = l;
            return true;
        }
        @endcode
    */
    virtual bool GetValueFromEditorCtrl(wxWindow* editor,
                                        wxVariant& value);

    /**
        Override this and make it return @true in order to
        indicate that this renderer supports in-place editing.
    */
    virtual bool HasEditorCtrl() const;

    /**
        Override this to react to a left click.
        This method will only be called in @c wxDATAVIEW_CELL_ACTIVATABLE mode.
    */
    virtual bool LeftClick( const wxPoint& cursor,
                            const wxRect& cell,
                            wxDataViewModel * model,
                            const wxDataViewItem & item,
                            unsigned int col );

    /**
        Override this to render the cell.
        Before this is called, wxDataViewRenderer::SetValue was called
        so that this instance knows what to render.
    */
    virtual bool Render(wxRect cell, wxDC* dc, int state) = 0;

    /**
        This method should be called from within Render() whenever you need to
        render simple text.
        This will ensure that the correct colour, font and vertical alignment will
        be chosen so the text will look the same as text drawn by native renderers.
    */
    void RenderText(const wxString& text, int xoffset, wxRect cell,
                    wxDC* dc, int state);

    /**
        Override this to start a drag operation. Not yet supported.
    */
    virtual bool StartDrag(const wxPoint& cursor,
                           const wxRect& cell,
                           wxDataViewModel* model,
                           const wxDataViewItem & item,
                           unsigned int col);
};



/**
    @class wxDataViewBitmapRenderer

    This class is used by wxDataViewCtrl to render bitmap controls.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewBitmapRenderer : public wxDataViewRenderer
{
public:
    /**
        The ctor.
    */
    wxDataViewBitmapRenderer(const wxString& varianttype = "wxBitmap",
                             wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                             int align = wxDVR_DEFAULT_ALIGNMENT);
};


/**
    The flags used by wxDataViewColumn.
    Can be combined together.
*/
enum wxDataViewColumnFlags
{
    wxDATAVIEW_COL_RESIZABLE     = 1,
    wxDATAVIEW_COL_SORTABLE      = 2,
    wxDATAVIEW_COL_REORDERABLE   = 4,
    wxDATAVIEW_COL_HIDDEN        = 8
};

/**
    @class wxDataViewColumn

    This class represents a column in a wxDataViewCtrl.
    One wxDataViewColumn is bound to one column in the data model to which the
    wxDataViewCtrl has been associated.

    An instance of wxDataViewRenderer is used by this class to render its data.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewColumn : public wxSettableHeaderColumn
{
public:
    /**
        Constructs a text column.

        @param title
            The title of the column.
        @param renderer
            The class which will render the contents of this column.
        @param model_column
            The index of the model's column which is associated with this object.
        @param width
            The width of the column.
            The @c wxDVC_DEFAULT_WIDTH value is the fixed default value.
        @param align
            The alignment of the column title.
        @param flags
            One or more flags of the ::wxDataViewColumnFlags enumeration.
    */
    wxDataViewColumn(const wxString& title,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);

    /**
        Constructs a bitmap column.

        @param bitmap
            The bitmap of the column.
        @param renderer
            The class which will render the contents of this column.
        @param model_column
            The index of the model's column which is associated with this object.
        @param width
            The width of the column.
            The @c wxDVC_DEFAULT_WIDTH value is the fixed default value.
        @param align
            The alignment of the column title.
        @param flags
            One or more flags of the ::wxDataViewColumnFlags enumeration.
    */
    wxDataViewColumn(const wxBitmap& bitmap,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);

    /**
        Returns the index of the column of the model, which this
        wxDataViewColumn is displaying.
    */
    unsigned int GetModelColumn() const;

    /**
        Returns the owning wxDataViewCtrl.
    */
    wxDataViewCtrl* GetOwner() const;

    /**
        Returns the renderer of this wxDataViewColumn.

        @see wxDataViewRenderer.
    */
    wxDataViewRenderer* GetRenderer() const;
};



/**
    @class wxDataViewListCtrl

    This class is a wxDataViewCtrl which internally uses a wxDataViewListStore
    and forwards most of its API to that class.

    The purpose of this class is to offer a simple way to display and
    edit a small table of data without having to write your own wxDataViewModel.

    @code
       wxDataViewListCtrl *listctrl = new wxDataViewListCtrl( parent, wxID_ANY );

       listctrl->AppendToggleColumn( "Toggle" );
       listctrl->AppendTextColumn( "Text" );

       wxVector<wxVariant> data;
       data.push_back( wxVariant(true) );
       data.push_back( wxVariant("row 1") );
       listctrl->AppendItem( data );

       data.clear();
       data.push_back( wxVariant(false) );
       data.push_back( wxVariant("row 3") );
       listctrl->AppendItem( data );
    @endcode

    @beginStyleTable
    See wxDataViewCtrl for the list of supported styles.
    @endStyleTable

    @beginEventEmissionTable
    See wxDataViewCtrl for the list of events emitted by this class.
    @endEventTable

    @library{wxadv}
    @category{ctrl,dvc}
*/
class wxDataViewListCtrl: public wxDataViewCtrl
{
public:
    /**
        Default ctor.
    */
    wxDataViewListCtrl();

    /**
        Constructor. Calls Create().
    */
    wxDataViewListCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = wxDV_ROW_LINES,
           const wxValidator& validator = wxDefaultValidator );

    /**
        Destructor. Deletes the image list if any.
    */
    ~wxDataViewListCtrl();

    /**
        Creates the control and a wxDataViewListStore as its internal model.
    */
    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = wxDV_ROW_LINES,
           const wxValidator& validator = wxDefaultValidator );

    //@{
    /**
        Returns the store.
    */
    wxDataViewListStore *GetStore();
    const wxDataViewListStore *GetStore() const;
    //@}

    /**
        Returns the position of given @e item or wxNOT_FOUND if it's
        not a valid item.

        @since 2.9.2
     */
    int ItemToRow(const wxDataViewItem &item) const;

    /**
        Returns the wxDataViewItem at the given @e row.

        @since 2.9.2
     */
    wxDataViewItem RowToItem(int row) const;

    //@{
    /**
        @name Selection handling functions
     */

    /**
        Returns index of the selected row or wxNOT_FOUND.

        @see wxDataViewCtrl::GetSelection()

        @since 2.9.2
     */
    int GetSelectedRow() const;

    /**
        Selects given row.

        @see wxDataViewCtrl::Select()

        @since 2.9.2
     */
    void SelectRow(unsigned row);

    /**
        Unselects given row.

        @see wxDataViewCtrl::Unselect()

        @since 2.9.2
     */
    void UnselectRow(unsigned row);

    /**
        Returns true if @a row is selected.

        @see wxDataViewCtrl::IsSelected()

        @since 2.9.2
     */
    bool IsRowSelected(unsigned row) const;

    //@}

    /**
        @name Column management functions
    */
    //@{

    /**
        Appends a column to the control and additionally appends a
        column to the store with the type string.
    */
    virtual bool AppendColumn( wxDataViewColumn *column );

    /**
        Appends a column to the control and additionally appends a
        column to the list store with the type @a varianttype.
    */
    void AppendColumn( wxDataViewColumn *column, const wxString &varianttype );

    /**
        Appends a text column to the control and the store.

        See wxDataViewColumn::wxDataViewColumn for more info about
        the parameters.
    */
    wxDataViewColumn *AppendTextColumn( const wxString &label,
          wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
          int width = -1, wxAlignment align = wxALIGN_LEFT,
          int flags = wxDATAVIEW_COL_RESIZABLE );

    /**
        Appends a toggle column to the control and the store.

        See wxDataViewColumn::wxDataViewColumn for more info about
        the parameters.
    */
    wxDataViewColumn *AppendToggleColumn( const wxString &label,
          wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
          int width = -1, wxAlignment align = wxALIGN_LEFT,
          int flags = wxDATAVIEW_COL_RESIZABLE );

    /**
        Appends a progress column to the control and the store.

        See wxDataViewColumn::wxDataViewColumn for more info about
        the parameters.
    */
    wxDataViewColumn *AppendProgressColumn( const wxString &label,
          wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
          int width = -1, wxAlignment align = wxALIGN_LEFT,
          int flags = wxDATAVIEW_COL_RESIZABLE );

    /**
        Appends an icon-and-text column to the control and the store.

        See wxDataViewColumn::wxDataViewColumn for more info about
        the parameters.
    */
    wxDataViewColumn *AppendIconTextColumn( const wxString &label,
          wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
          int width = -1, wxAlignment align = wxALIGN_LEFT,
          int flags = wxDATAVIEW_COL_RESIZABLE );

    /**
        Inserts a column to the control and additionally inserts a
        column to the store with the type string.
    */
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *column );

    /**
        Inserts a column to the control and additionally inserts a
        column to the list store with the type @a varianttype.
    */
    void InsertColumn( unsigned int pos, wxDataViewColumn *column,
                       const wxString &varianttype );

    /**
        Prepends a column to the control and additionally prepends a
        column to the store with the type string.
    */
    virtual bool PrependColumn( wxDataViewColumn *column );

    /**
        Prepends a column to the control and additionally prepends a
        column to the list store with the type @a varianttype.
    */
    void PrependColumn( wxDataViewColumn *column, const wxString &varianttype );

    //@}


    /**
        @name Item management functions
    */
    //@{

    /**
        Appends an item (=row) to the control and store.
    */
    void AppendItem( const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Prepends an item (=row) to the control and store.
    */
    void PrependItem( const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Inserts an item (=row) to the control and store.
    */
    void InsertItem( unsigned int row, const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Delete the row at position @a row.
    */
    void DeleteItem( unsigned row );

    /**
        Delete all items (= all rows).
    */
    void DeleteAllItems();

    /**
        Returns the number of items (=rows) in the control

        @since 2.9.4
    */
    unsigned int GetItemCount() const;

    /**
        Returns the client data associated with the item.

        @see SetItemData()

        @since 2.9.4
    */
    wxUIntPtr GetItemData(const wxDataViewItem& item) const;

    /**
         Sets the value in the store and update the control.
    */
    void SetValue( const wxVariant &value, unsigned int row, unsigned int col );

    /**
         Returns the value from the store.
    */
    void GetValue( wxVariant &value, unsigned int row, unsigned int col );

    /**
         Sets the value in the store and update the control.

         This method assumes that the string is stored in respective
         column.
    */
    void SetTextValue( const wxString &value, unsigned int row, unsigned int col );

    /**
         Returns the value from the store.

         This method assumes that the string is stored in respective
         column.
    */
    wxString GetTextValue( unsigned int row, unsigned int col ) const;

    /**
         Sets the value in the store and update the control.

         This method assumes that the boolean value is stored in
         respective column.
    */
    void SetToggleValue( bool value, unsigned int row, unsigned int col );

    /**
         Returns the value from the store.

         This method assumes that the boolean value is stored in
         respective column.
    */
    bool GetToggleValue( unsigned int row, unsigned int col ) const;

    /**
        Associates a client data pointer with the given item.

        Notice that the control does @e not take ownership of the pointer for
        compatibility with wxListCtrl. I.e. it will @e not delete the pointer
        (if it is a pointer and not a number) itself, it is up to you to do it.

        @see GetItemData()

        @since 2.9.4
    */
    void SetItemData(const wxDataViewItem& item, wxUIntPtr data);

    //@}
};


/**
    @class wxDataViewTreeCtrl

    This class is a wxDataViewCtrl which internally uses a wxDataViewTreeStore
    and forwards most of its API to that class.
    Additionally, it uses a wxImageList to store a list of icons.

    The main purpose of this class is to provide a simple upgrade path for code
    using wxTreeCtrl.

    @beginStyleTable
    See wxDataViewCtrl for the list of supported styles.
    @endStyleTable

    @beginEventEmissionTable
    See wxDataViewCtrl for the list of events emitted by this class.
    @endEventTable

    @library{wxadv}
    @category{ctrl,dvc}
    @appearance{dataviewtreectrl.png}
*/
class wxDataViewTreeCtrl : public wxDataViewCtrl
{
public:
    /**
        Default ctor.
    */
    wxDataViewTreeCtrl();

    /**
        Constructor.

        Calls Create().
    */
    wxDataViewTreeCtrl(wxWindow* parent, wxWindowID id,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDV_NO_HEADER | wxDV_ROW_LINES,
                       const wxValidator& validator = wxDefaultValidator);

    /**
        Destructor. Deletes the image list if any.
    */
    virtual ~wxDataViewTreeCtrl();

    /**
        Appends a container to the given @a parent.
    */
    wxDataViewItem AppendContainer(const wxDataViewItem& parent,
                                   const wxString& text,
                                   int icon = -1,
                                   int expanded = -1,
                                   wxClientData* data = NULL);

    /**
        Appends an item to the given @a parent.
    */
    wxDataViewItem AppendItem(const wxDataViewItem& parent,
                              const wxString& text,
                              int icon = -1,
                              wxClientData* data = NULL);

    /**
        Creates the control and a wxDataViewTreeStore as its internal model.

        The default tree column created by this method is an editable column
        using wxDataViewIconTextRenderer as its renderer.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDV_NO_HEADER | wxDV_ROW_LINES,
                const wxValidator& validator = wxDefaultValidator);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void DeleteAllItems();

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void DeleteChildren(const wxDataViewItem& item);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void DeleteItem(const wxDataViewItem& item);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    int GetChildCount(const wxDataViewItem& parent) const;

    /**
        Returns the image list.
    */
    wxImageList* GetImageList();

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    wxClientData* GetItemData(const wxDataViewItem& item) const;

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    const wxIcon& GetItemExpandedIcon(const wxDataViewItem& item) const;

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    const wxIcon& GetItemIcon(const wxDataViewItem& item) const;

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    wxString GetItemText(const wxDataViewItem& item) const;

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    wxDataViewItem GetNthChild(const wxDataViewItem& parent,
                               unsigned int pos) const;

    //@{
    /**
        Returns the store.
    */
    wxDataViewTreeStore* GetStore();
    const wxDataViewTreeStore* GetStore() const;
    //@}

    /**
        Calls the same method from wxDataViewTreeStore but uses
        an index position in the image list instead of a wxIcon.
    */
    wxDataViewItem InsertContainer(const wxDataViewItem& parent,
                                   const wxDataViewItem& previous,
                                   const wxString& text,
                                   int icon = -1,
                                   int expanded = -1,
                                   wxClientData* data = NULL);

    /**
        Calls the same method from wxDataViewTreeStore but uses
        an index position in the image list instead of a wxIcon.
    */
    wxDataViewItem InsertItem(const wxDataViewItem& parent,
                              const wxDataViewItem& previous,
                              const wxString& text,
                              int icon = -1,
                              wxClientData* data = NULL);

    /**
        Returns true if item is a container.
    */
    bool IsContainer( const wxDataViewItem& item );
    
    /**
        Calls the same method from wxDataViewTreeStore but uses
        an index position in the image list instead of a wxIcon.
    */
    wxDataViewItem PrependContainer(const wxDataViewItem& parent,
                                    const wxString& text,
                                    int icon = -1,
                                    int expanded = -1,
                                    wxClientData* data = NULL);

    /**
        Calls the same method from wxDataViewTreeStore but uses
        an index position in the image list instead of a wxIcon.
    */
    wxDataViewItem PrependItem(const wxDataViewItem& parent,
                               const wxString& text,
                               int icon = -1,
                               wxClientData* data = NULL);

    /**
        Sets the image list.
    */
    void SetImageList(wxImageList* imagelist);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void SetItemData(const wxDataViewItem& item, wxClientData* data);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void SetItemExpandedIcon(const wxDataViewItem& item,
                             const wxIcon& icon);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void SetItemIcon(const wxDataViewItem& item, const wxIcon& icon);

    /**
        Calls the identical method from wxDataViewTreeStore.
    */
    void SetItemText(const wxDataViewItem& item,
                     const wxString& text);
};


/**
    @class wxDataViewListStore

    wxDataViewListStore is a specialised wxDataViewModel for storing
    a simple table of data. Since it derives from wxDataViewIndexListModel
    its data is be accessed by row (i.e. by index) instead of only
    by wxDataViewItem.

    This class actually stores the values (therefore its name)
    and implements all virtual methods from the base classes so it can be
    used directly without having to derive any class from it, but it is
    mostly used from within wxDataViewListCtrl.

    @library{wxadv}
    @category{dvc}
*/

class wxDataViewListStore: public wxDataViewIndexListModel
{
public:
    /**
        Constructor
    */
    wxDataViewListStore();

    /**
        Destructor
    */
    ~wxDataViewListStore();

    /**
        Prepends a data column.

        @a variantype indicates the type of values store in the column.

        This does not automatically fill in any (default) values in
        rows which exist in the store already.
    */
    void PrependColumn( const wxString &varianttype );

    /**
        Inserts a data column before @a pos.

        @a variantype indicates the type of values store in the column.

        This does not automatically fill in any (default) values in
        rows which exist in the store already.
    */
    void InsertColumn( unsigned int pos, const wxString &varianttype );

    /**
        Appends a data column.

        @a variantype indicates the type of values store in the column.

        This does not automatically fill in any (default) values in
        rows which exist in the store already.
    */
    void AppendColumn( const wxString &varianttype );

    /**
        Appends an item (=row) and fills it with @a values.

        The values must match the values specifies in the column
        in number and type. No (default) values are filled in
        automatically.
    */
    void AppendItem( const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Prepends an item (=row) and fills it with @a values.

        The values must match the values specifies in the column
        in number and type. No (default) values are filled in
        automatically.
    */
    void PrependItem( const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Inserts an item (=row) and fills it with @a values.

        The values must match the values specifies in the column
        in number and type. No (default) values are filled in
        automatically.
    */
    void InsertItem(  unsigned int row, const wxVector<wxVariant> &values, wxUIntPtr data = NULL );

    /**
        Delete the item (=row) at position @a pos.
    */
    void DeleteItem( unsigned pos );

    /**
        Delete all item (=all rows) in the store.
    */
    void DeleteAllItems();

    /**
        Returns the number of items (=rows) in the control

        @since 2.9.4
    */
    unsigned int GetItemCount() const;

    /**
        Returns the client data associated with the item.

        @see SetItemData()

        @since 2.9.4
    */
    wxUIntPtr GetItemData(const wxDataViewItem& item) const;

    /**
        Overridden from wxDataViewModel
    */
    virtual unsigned int GetColumnCount() const;

    /**
        Overridden from wxDataViewModel
    */
    virtual wxString GetColumnType( unsigned int col ) const;

    /**
        Sets the client data associated with the item.

        Notice that this class does @e not take ownership of the passed in
        pointer and will not delete it.

        @see GetItemData()

        @since 2.9.4
    */
    void SetItemData(const wxDataViewItem& item, wxUIntPtr data);

    /**
        Overridden from wxDataViewIndexListModel
    */
    virtual void GetValueByRow( wxVariant &value,
                           unsigned int row, unsigned int col ) const;

    /**
        Overridden from wxDataViewIndexListModel
    */
    virtual bool SetValueByRow( const wxVariant &value,
                           unsigned int row, unsigned int col );
};


/**
    @class wxDataViewTreeStore

    wxDataViewTreeStore is a specialised wxDataViewModel for storing simple
    trees very much like wxTreeCtrl does and it offers a similar API.

    This class actually stores the entire tree and the values (therefore its name)
    and implements all virtual methods from the base class so it can be used directly
    without having to derive any class from it, but it is mostly used from within
    wxDataViewTreeCtrl.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewTreeStore : public wxDataViewModel
{
public:
    /**
        Constructor. Creates the invisible root node internally.
    */
    wxDataViewTreeStore();

    /**
        Destructor.
    */
    virtual ~wxDataViewTreeStore();

    /**
        Append a container.
    */
    wxDataViewItem AppendContainer(const wxDataViewItem& parent,
                                   const wxString& text,
                                   const wxIcon& icon = wxNullIcon,
                                   const wxIcon& expanded = wxNullIcon,
                                   wxClientData* data = NULL);

    /**
        Append an item.
    */
    wxDataViewItem AppendItem(const wxDataViewItem& parent,
                              const wxString& text,
                              const wxIcon& icon = wxNullIcon,
                              wxClientData* data = NULL);

    /**
        Delete all item in the model.
    */
    void DeleteAllItems();

    /**
        Delete all children of the item, but not the item itself.
    */
    void DeleteChildren(const wxDataViewItem& item);

    /**
        Delete this item.
    */
    void DeleteItem(const wxDataViewItem& item);

    /**
        Return the number of children of item.
    */
    int GetChildCount(const wxDataViewItem& parent) const;

    /**
        Returns the client data associated with the item.
    */
    wxClientData* GetItemData(const wxDataViewItem& item) const;

    /**
        Returns the icon to display in expanded containers.
    */
    const wxIcon& GetItemExpandedIcon(const wxDataViewItem& item) const;

    /**
        Returns the icon of the item.
    */
    const wxIcon& GetItemIcon(const wxDataViewItem& item) const;

    /**
        Returns the text of the item.
    */
    wxString GetItemText(const wxDataViewItem& item) const;

    /**
        Returns the nth child item of item.
    */
    wxDataViewItem GetNthChild(const wxDataViewItem& parent,
                               unsigned int pos) const;

    /**
        Inserts a container after @a previous.
    */
    wxDataViewItem InsertContainer(const wxDataViewItem& parent,
                                   const wxDataViewItem& previous,
                                   const wxString& text,
                                   const wxIcon& icon = wxNullIcon,
                                   const wxIcon& expanded = wxNullIcon,
                                   wxClientData* data = NULL);

    /**
        Inserts an item after @a previous.
    */
    wxDataViewItem InsertItem(const wxDataViewItem& parent,
                              const wxDataViewItem& previous,
                              const wxString& text,
                              const wxIcon& icon = wxNullIcon,
                              wxClientData* data = NULL);

    /**
        Inserts a container before the first child item or @a parent.
    */
    wxDataViewItem PrependContainer(const wxDataViewItem& parent,
                                    const wxString& text,
                                    const wxIcon& icon = wxNullIcon,
                                    const wxIcon& expanded = wxNullIcon,
                                    wxClientData* data = NULL);

    /**
        Inserts an item before the first child item or @a parent.
    */
    wxDataViewItem PrependItem(const wxDataViewItem& parent,
                               const wxString& text,
                               const wxIcon& icon = wxNullIcon,
                               wxClientData* data = NULL);

    /**
        Sets the client data associated with the item.
    */
    void SetItemData(const wxDataViewItem& item, wxClientData* data);

    /**
        Sets the expanded icon for the item.
    */
    void SetItemExpandedIcon(const wxDataViewItem& item,
                             const wxIcon& icon);

    /**
        Sets the icon for the item.
    */
    void SetItemIcon(const wxDataViewItem& item, const wxIcon& icon);
};


/**
    @class wxDataViewIconText

    wxDataViewIconText is used by wxDataViewIconTextRenderer for data transfer.
    This class can be converted to and from a wxVariant.

    @library{wxadv}
    @category{dvc}
*/
class wxDataViewIconText : public wxObject
{
public:
    //@{
    /**
        Constructor.
    */
    wxDataViewIconText(const wxString& text = wxEmptyString,
                       const wxIcon& icon = wxNullIcon);
    wxDataViewIconText(const wxDataViewIconText& other);
    //@}

    /**
        Gets the icon.
    */
    const wxIcon& GetIcon() const;

    /**
        Gets the text.
    */
    wxString GetText() const;

    /**
        Set the icon.
    */
    void SetIcon(const wxIcon& icon);

    /**
        Set the text.
    */
    void SetText(const wxString& text);
};



/**
    @class wxDataViewEvent

    This is the event class for the wxDataViewCtrl notifications.

    @beginEventTable{wxDataViewEvent}
    @event{EVT_DATAVIEW_SELECTION_CHANGED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED event.
    @event{EVT_DATAVIEW_ITEM_ACTIVATED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED event.
    @event{EVT_DATAVIEW_ITEM_EDITING_STARTED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED event.
    @event{EVT_DATAVIEW_ITEM_EDITING_DONE(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE event.
    @event{EVT_DATAVIEW_ITEM_COLLAPSING(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING event.
    @event{EVT_DATAVIEW_ITEM_COLLAPSED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED event.
    @event{EVT_DATAVIEW_ITEM_EXPANDING(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING event.
    @event{EVT_DATAVIEW_ITEM_EXPANDED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED event.
    @event{EVT_DATAVIEW_ITEM_VALUE_CHANGED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED event.
    @event{EVT_DATAVIEW_ITEM_CONTEXT_MENU(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU event.
    @event{EVT_DATAVIEW_COLUMN_HEADER_CLICK(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK event.
    @event{EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED event.
    @event{EVT_DATAVIEW_COLUMN_SORTED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED event.
    @event{EVT_DATAVIEW_COLUMN_REORDERED(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED event.
           Currently this even is only generated when using the native OSX
           version.
    @event{EVT_DATAVIEW_ITEM_BEGIN_DRAG(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG event.
    @event{EVT_DATAVIEW_ITEM_DROP_POSSIBLE(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE event.
    @event{EVT_DATAVIEW_ITEM_DROP(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_ITEM_DROP event.
    @event{EVT_DATAVIEW_CACHE_HINT(id, func)}
           Process a @c wxEVT_COMMAND_DATAVIEW_CACHE_HINT event.
    @endEventTable

    @library{wxadv}
    @category{events,dvc}
*/
class wxDataViewEvent : public wxNotifyEvent
{
public:
    /**
        Constructor. Typically used by wxWidgets internals only.
    */
    wxDataViewEvent(wxEventType commandType = wxEVT_NULL,
                    int winid = 0);

    /**
        Returns the position of the column in the control or -1
        if no column field was set by the event emitter.
    */
    int GetColumn() const;

    /**
        Returns a pointer to the wxDataViewColumn from which
        the event was emitted or @NULL.
    */
    wxDataViewColumn* GetDataViewColumn() const;

    /**
        Returns the wxDataViewModel associated with the event.
    */
    wxDataViewModel* GetModel() const;

    /**
        Returns the position of a context menu event in screen coordinates.
    */
    wxPoint GetPosition() const;

    /**
        Returns a reference to a value.
    */
    const wxVariant& GetValue() const;

    /**
        Can be used to determine whether the new value is going to be accepted
        in wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE handler.

        Returns @true if editing the item was cancelled or if the user tried to
        enter an invalid value (refused by wxDataViewRenderer::Validate()). If
        this method returns @false, it means that the value in the model is
        about to be changed to the new one.

        Notice that wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE event handler can
        call wxNotifyEvent::Veto() to prevent this from happening.

        Currently support for setting this field and for vetoing the change is
        only available in the generic version of wxDataViewCtrl, i.e. under MSW
        but not GTK nor OS X.

        @since 2.9.3
     */
    bool IsEditCancelled() const;

    /**
        Sets the column index associated with this event.
    */
    void SetColumn(int col);

    /**
        For @c wxEVT_DATAVIEW_COLUMN_HEADER_CLICK only.
    */
    void SetDataViewColumn(wxDataViewColumn* col);

    /**
        Sets the dataview model associated with this event.
    */
    void SetModel(wxDataViewModel* model);

    /**
        Sets the value associated with this event.
    */
    void SetValue(const wxVariant& value);

    /**
        Set wxDataObject for data transfer within a drag operation.
    */
    void SetDataObject( wxDataObject *obj );

    /**
        Gets the wxDataFormat during a drop operation.
    */
    wxDataFormat GetDataFormat() const;

    /**
        Gets the data size for a drop data transfer.
    */
    size_t GetDataSize() const;

    /**
        Gets the data buffer for a drop data transfer.
    */
    void *GetDataBuffer() const;

    /**
        Specify the kind of the drag operation to perform.

        This method can be used inside a wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG
        handler in order to configure the drag operation. Valid values are
        ::wxDrag_CopyOnly (default), ::wxDrag_AllowMove (allow the data to be
        moved) and ::wxDrag_DefaultMove.

        Currently it is only honoured by the generic version of wxDataViewCtrl
        (used e.g. under MSW) and not supported by the native GTK and OS X
        versions.

        @see GetDropEffect()

        @since 2.9.4
    */
    void SetDragFlags(int flags);

    /**
        Returns the effect the user requested to happen to the dropped data.

        This function can be used inside
        wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE and
        wxEVT_COMMAND_DATAVIEW_ITEM_DROP handlers and returns whether the user
        is trying to copy (the return value is ::wxDragCopy) or move (if the
        return value is ::wxDragMove) the data.

        Currently this is only available when using the generic version of
        wxDataViewCtrl (used e.g. under MSW) and always returns ::wxDragNone in
        the GTK and OS X native versions.

        @since 2.9.4
    */
    wxDragResult GetDropEffect() const;

    /**
        Return the first row that will be displayed.
    */
    int GetCacheFrom() const;

    /**
        Return the last row that will be displayed.
    */
    int GetCacheTo() const;
};

