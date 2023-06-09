/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrl.h
// Purpose:     interface of wxSpinCtrl
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSpinCtrl

    wxSpinCtrl combines wxTextCtrl and wxSpinButton in one control.

    @beginStyleTable
    @style{wxSP_ARROW_KEYS}
        The user can use arrow keys to change the value.
    @style{wxSP_WRAP}
        The value wraps at the minimum and maximum.
    @style{wxTE_PROCESS_ENTER}
        Indicates that the control should generate @c wxEVT_COMMAND_TEXT_ENTER
        events. Using this style will prevent the user from using the Enter key
        for dialog navigation (e.g. activating the default button in the
        dialog) under MSW.
    @style{wxALIGN_LEFT}
        Same as wxTE_LEFT for wxTextCtrl: the text is left aligned.
    @style{wxALIGN_CENTRE_HORIZONTAL}
        Same as wxTE_CENTRE for wxTextCtrl: the text is centered.
    @style{wxALIGN_RIGHT}
        Same as wxTE_RIGHT for wxTextCtrl: the text is right aligned (this is
        the default).
    @endStyleTable


    @beginEventEmissionTable{wxSpinEvent}
    @event{EVT_SPINCTRL(id, func)}
        Process a wxEVT_COMMAND_SPINCTRL_UPDATED event, which is generated
        whenever the numeric value of the spin control is updated.
    @endEventTable

    You may also use the wxSpinButton event macros, however the corresponding events
    will not be generated under all platforms. Finally, if the user modifies the
    text in the edit part of the spin control directly, the EVT_TEXT is generated,
    like for the wxTextCtrl. When the use enters text into the text area, the text
    is not validated until the control loses focus (e.g. by using the TAB key).
    The value is then adjusted to the range and a wxSpinEvent sent then if the value
    is different from the last value sent.

    @library{wxcore}
    @category{ctrl}
    @appearance{spinctrl.png}

    @see wxSpinButton, wxSpinCtrlDouble, wxControl
*/
class wxSpinCtrl : public wxControl
{
public:
    /**
       Default constructor.
    */
    wxSpinCtrl();

    /**
        Constructor, creating and showing a spin control.

        If @a value is non-empty, it will be shown in the text entry part of
        the control and if it has numeric value, the initial numeric value of
        the control, as returned by GetValue() will also be determined by it
        instead of by @a initial. Hence, it only makes sense to specify @a
        initial if @a value is an empty string or is not convertible to a
        number, otherwise @a initial is simply ignored and the number specified
        by @a value is used.

        @param parent
            Parent window. Must not be @NULL.
        @param value
            Default value (as text).
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position.
            If ::wxDefaultPosition is specified then a default position is chosen.
        @param size
            Window size.
            If ::wxDefaultSize is specified then a default size is chosen.
        @param style
            Window style. See wxSpinButton.
        @param min
            Minimal value.
        @param max
            Maximal value.
        @param initial
            Initial value.
        @param name
            Window name.

        @see Create()
    */
    wxSpinCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100,
               int initial = 0, const wxString& name = "wxSpinCtrl");

    /**
        Creation function called by the spin control constructor.
        See wxSpinCtrl() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS, int min = 0, int max = 100,
                int initial = 0, const wxString& name = "wxSpinCtrl");

    /**
        Gets maximal allowable value.
    */
    int GetMax() const;

    /**
        Gets minimal allowable value.
    */
    int GetMin() const;

    /**
        Gets the value of the spin control.
    */
    int GetValue() const;

    /**
        Sets range of allowable values.
    */
    void SetRange(int minVal, int maxVal);

    /**
        Select the text in the text part of the control between  positions
        @a from (inclusive) and @a to (exclusive).
        This is similar to wxTextCtrl::SetSelection().

        @note this is currently only implemented for Windows and generic versions
              of the control.
    */
    virtual void SetSelection(long from, long to);

    /**
        Sets the value of the spin control. Use the variant using int instead.
    */
    virtual void SetValue(const wxString& text);

    /**
        Sets the value of the spin control.
    */
    void SetValue(int value);
};

/**
    @class wxSpinCtrlDouble

    wxSpinCtrlDouble combines wxTextCtrl and wxSpinButton in one control and
    displays a real number. (wxSpinCtrl displays an integer.)

    @beginStyleTable
    @style{wxSP_ARROW_KEYS}
           The user can use arrow keys to change the value.
    @style{wxSP_WRAP}
           The value wraps at the minimum and maximum.
    @endStyleTable

    @beginEventEmissionTable{wxSpinDoubleEvent}
    @event{EVT_SPINCTRLDOUBLE(id, func)}
        Generated whenever the numeric value of the spin control is changed,
        that is, when the up/down spin button is clicked, when ENTER is pressed,
        or the control loses focus and the new value is different from the last.
        See wxSpinDoubleEvent.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{spinctrldouble.png}

    @see wxSpinButton, wxSpinCtrl, wxControl
*/
class wxSpinCtrlDouble : public wxControl
{
public:
    /**
       Default constructor.
    */
    wxSpinCtrlDouble();

    /**
        Constructor, creating and showing a spin control.

        @param parent
            Parent window. Must not be @NULL.
        @param value
            Default value (as text).
        @param id
            Window identifier. The value wxID_ANY indicates a default value.
        @param pos
            Window position.
            If ::wxDefaultPosition is specified then a default position is chosen.
        @param size
            Window size.
            If ::wxDefaultSize is specified then a default size is chosen.
        @param style
            Window style. See wxSpinButton.
        @param min
            Minimal value.
        @param max
            Maximal value.
        @param initial
            Initial value.
        @param inc
            Increment value.
        @param name
            Window name.

        @see Create()
    */
    wxSpinCtrlDouble(wxWindow* parent, wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               double min = 0, double max = 100,
               double initial = 0, double inc = 1,
               const wxString& name = wxT("wxSpinCtrlDouble"));

    /**
        Creation function called by the spin control constructor.
        See wxSpinCtrlDouble() for details.
    */
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS, double min = 0, double max = 100,
                double initial = 0, double inc = 1,
                const wxString& name = "wxSpinCtrlDouble");

    /**
        Gets the number of digits in the display.
    */
    unsigned int GetDigits() const;

    /**
        Gets the increment value.
    */
    double GetIncrement() const;

    /**
        Gets maximal allowable value.
    */
    double GetMax() const;

    /**
        Gets minimal allowable value.
    */
    double GetMin() const;

    /**
        Gets the value of the spin control.
    */
    double GetValue() const;

    /**
        Sets the number of digits in the display.
    */
    void SetDigits(unsigned int digits);

    /**
        Sets the increment value.
    */
    void SetIncrement(double inc);

    /**
        Sets range of allowable values.
    */
    void SetRange(double minVal, double maxVal);

    /**
        Sets the value of the spin control. Use the variant using double instead.
    */
    virtual void SetValue(const wxString& text);

    /**
        Sets the value of the spin control.
    */
    void SetValue(double value);
};

/**
    @class wxSpinDoubleEvent

    This event class is used for the events generated by wxSpinCtrlDouble.

    @beginEventTable{wxSpinDoubleEvent}
    @event{EVT_SPINCTRLDOUBLE(id, func)}
        Generated whenever the numeric value of the spin control is changed,
        that is, when the up/down spin button is clicked, when ENTER is pressed,
        or the control loses focus and the new value is different from the last.
        See wxSpinDoubleEvent.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxSpinCtrlDouble
*/
class wxSpinDoubleEvent : public wxNotifyEvent
{
public:
    /**
        The constructor. Not normally used by the user code.
    */
    wxSpinDoubleEvent(wxEventType commandType = wxEVT_NULL, int winid = 0,
                      double value = 0);

    /**
        The copy constructor.
    */
    wxSpinDoubleEvent(const wxSpinDoubleEvent& event);

    /**
        Returns the value associated with this spin control event.
    */
    double GetValue() const;

    /**
        Set the value associated with the event.
        (Not normally used by user code.)
    */
    void SetValue(double value);
};

wxEventType wxEVT_COMMAND_SPINCTRL_UPDATED;
wxEventType wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED;
