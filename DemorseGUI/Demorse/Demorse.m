function varargout = Demorse(varargin)
% DEMORSE MATLAB code for Demorse.fig
%      DEMORSE, by itself, creates a new DEMORSE or raises the existing
%      singleton*.
%
%      H = DEMORSE returns the handle to a new DEMORSE or the handle to
%      the existing singleton*.
%
%      DEMORSE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in DEMORSE.M with the given input arguments.
%
%      DEMORSE('Property','Value',...) creates a new DEMORSE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before Demorse_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to Demorse_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help Demorse

% Last Modified by GUIDE v2.5 18-Sep-2016 01:09:00

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @Demorse_OpeningFcn, ...
                   'gui_OutputFcn',  @Demorse_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before Demorse is made visible.
function Demorse_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to Demorse (see VARARGIN)

% Choose default command line output for Demorse
handles.output = hObject;
handles.audiofile_path = '';
handles.audiofile_name = '';
handles.file_loaded = false;
handles.full_file_path = '';
handles.decoded_value = '';

% Widget reference
handles.decodedEditText = findobj('Tag', 'decodedEditText');
handles.resetButton = findobj('Tag', 'resetButton');
handles.statusLabel = findobj('Tag', 'statusLabel');
handles.decodeButton = findobj('Tag', 'decodeButton');
handles.loadButton = findobj('Tag', 'loadButton');
handles.filepathLabel = findobj('Tag', 'filepathLabel');

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes Demorse wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = Demorse_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in loadButton.
function loadButton_Callback(hObject, eventdata, handles)
% hObject    handle to loadButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[handles.audiofile_name, handles.audiofile_path] = uigetfile({'*.wav'; '*.mp3'; '*.oog';'*.amr'; '*.m4a'}, 'Load an morse encoded audiofile')
% Debugging purpose
disp(handles.audiofile_name)
disp(handles.audiofile_path)
handles.full_file_path = [handles.audiofile_path handles.audiofile_name]
handles.full_file_path
status = 'File loaded successfully';
set(handles.statusLabel, 'String', status)
set(handles.filepathLabel, 'String', handles.full_file_path)
handles.file_loaded = true;
% Saving all data
guidata(hObject, handles)



function decodedEditText_Callback(hObject, eventdata, handles)
% hObject    handle to decodedEditText (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of decodedEditText as text
%        str2double(get(hObject,'String')) returns contents of decodedEditText as a double



% --- Executes during object creation, after setting all properties.
function decodedEditText_CreateFcn(hObject, eventdata, handles)
% hObject    handle to decodedEditText (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in decodeButton.
function decodeButton_Callback(hObject, eventdata, handles)
% hObject    handle to decodeButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% handles.full_file_path
% handles.audiofile_path
if handles.file_loaded == true
    handles.decoded_value = demorse_function(handles.full_file_path)
    set(handles.decodedEditText, 'String', handles.decoded_value)
else
    warning('File is not supported or not loaded properly')
    errordlg('File is not loaded!', 'File not found', 'modal')
    return
end

guidata(hObject, handles)
% disp(demorse_function(handles.full_file_path))
    


% --- Executes on button press in resetButton.
function resetButton_Callback(hObject, eventdata, handles)
% hObject    handle to resetButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.file_loaded = false;
handles.audiofile_path = '';
handles.audiofile_name = '';
handles.full_file_path = '';
handles.decoded_value = '';
set(handles.statusLabel, 'String', 'Load a Morse Encoded Audio File')
set(handles.decodedEditText, 'String', 'Decoded text will be shown here')
set(handles.filepathLabel, 'String', 'File Path')
guidata(hObject, handles)


% --- Executes on button press in playButton.
function playButton_Callback(hObject, eventdata, handles)
% hObject    handle to playButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[audiofile, sample_freq] = audioread(handles.full_file_path);
sound(audiofile, sample_freq);