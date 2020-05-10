// dear imgui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>
#if (WIN32)
#include <Windows.h>
#endif

#include <array>
#include <tuple>
#include <string>
#include "RtMidi.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

const std::array<const std::tuple<const std::string, const uint8_t>, 4> BANK_CC{
        std::make_tuple("A", 0),
        std::make_tuple("B", 1),
        std::make_tuple("C", 10),
        std::make_tuple("D", 11),
};

std::array<std::array<std::string, 128>, 4> INSTRUMENT_PC
{ {
    {"1 Grand Piano St", "2 Rock Piano St", "3 Electric St", "4 Honky-tonk St", "5 El.Piano1", "6 El.Piano2", "7 Harpsichord", "8 Clavi", "9 Celesta", "10 Glockenspiel", "11 MusicBox", "12 Vibraphone", "13 Marimba", "14 Xylophone", "15 Tubular bells", "16 Dulcimer", "17 Rotary", "18 Percussive", "19 Rock Organ", "20 Church", "21 Theatre", "22 Musette", "23 Harmonica", "24 Bandoneon", "25 Nylon", "26 Steel M", "27 Jazz", "28 Clean", "29 Muted", "30 Overdrive", "31 Distorted", "32 Harmonics", "33 Acoustic", "34 Finger", "35 Picked", "36 Fretless", "37 Slap Bass1", "38 Slap Bass2", "39 Synbas1", "40 Synbas2", "41 Violin", "42 Viola", "43 Cello", "44 Contrabass", "45 Tremolo", "46 Pizzicato", "47 Harp", "48 Timpani", "49 Strings Ens. St", "50 SlowStrings St", "51 SynStrings1", "52 SynStrings2", "53 Choir Aah St, M", "54 Voice Ooh", "55 Synth Voice", "56 Hits", "57 Trumpet", "58 Trombone", "59 Tuba", "60 Muted Tpt", "61 French Horn", "62 Brass Sect.", "63 SynBrass1", "64 SynBrass2", "65 Altosoft", "66 Contralto", "67 Tenor Sax", "68 Bariton Sax", "69 Oboe", "70 Engl.Horn", "71 Bassoon", "72 Clarinet", "73 Piccolo", "74 Flute", "75 Recorder", "76 Pan Flute", "77 Bottle", "78 Shakuhashi", "79 Whistle", "80 Ocarina", "81 Square", "82 Sawtooth", "83 Calliope", "84 Chiff", "85 Charang", "86 Voice St", "87 Fifth", "88 Bass&Lead", "89 Fantasy St", "90 Warm St", "91 New Age", "92 Space", "93 Soft", "94 Metal St", "95 Halo", "96 Sweep", "97 Ice Rain", "98 Soundtrack", "99 Crystal", "100 Atmosphere", "101 Brightness", "102 Goblin", "103 Echodrops", "104 Startheme", "105 Sitar", "106 Banjo", "107 Shamisen", "108 Koto", "109 Kalimba", "110 Bagpipe", "111 Fiddle", "112 Shanai", "113 Tinkle bell", "114 Agogo", "115 Steeldrum St", "116 Woodblock", "117 Taiko", "118 Melotom", "119 Syntom", "120 Reverse", "121 Fret Noise", "122 Breath", "123 Seashore", "124 Bird", "125 Telephone", "126 Helicopter", "127 Applause", "128 Gunshot"},
    {"1 Jingle", "2 Dark Grand St", "3 House St", "4 Suitcase", "5 Mark", "6 Funk Dx", "7 Fm Tine", "8 DX Piano", "9 ToyPad", "10 Orch. Bell", "11 Mallet", "12 Short Vibe", "13 Karimba", "14 Etnowood", "15 Metallophone", "16 Windchimes", "17 Rotor Fast", "18 Jazz Organ", "19 Drawbar", "20 Positive", "21 French Musette", "22 Francaise", "23 Cassotto", "24 Tango", "25 Classic", "26 Folk", "27 Funk Strat", "28 60’ Stopped", "29 Muted velo", "30 Vintage", "31 12 Strings", "32 Solidbody", "33 WarmBass", "34 Folk Bass", "35 MuteBass", "36 Deep Bass", "37 Precision", "38 Reso Bass", "39 Saw Bass", "40 House Bass", "41 Philarmonic", "42 Quartet", "43 GlockString", "44 OctaStrings1", "45 OctaStrings2", "46 Marcato", "47 String&Fall St", "48 Slow Arcos St", "49 Bowed Strings St", "50 Dark Strings St", "51 Dah", "52 Classic Choir", "53 Men_Doo", "54 Girl_Doo St", "55 Laah", "56 Voices", "57 Swing Tpt", "58 Cornet", "59 Euphonium", "60 Miles Tpt", "61 Flughorn", "62 Stereo Brass St", "63 GrowBras", "64 Octabrass", "65 Soprano", "66 Circus Sax", "67 Lounge Tenor", "68 Growl Sax", "69 Melo Trumpet", "70 Horn Swell", "71 Trumpets St", "72 Soft Trombones St", "73 Latin Flute St", "74 Pop Flute", "75 Andes", "76 Voice Flute", "77 Analogs St", "78 Flanger", "79 Brightpad", "80 Wiring St", "81 Wow St", "82 Polysynth St", "83 Trance St", "84 Wave", "85 Rave Bass", "86 Modular Bass St", "87 Percussive Bass St", "88 Wild Bass St", "89 2nd Perc", "90 3th Perc", "91 Power Fast", "92 Tonebar", "93 Dark B3", "94 Jimmy Org.", "95 Lower 8’ - 4’", "96 Jazzy", "97 Pop Strat", "98 Telecast", "99 Strato", "100 Texas", "101 Western", "102 Fingerpick", "103 5th Stops", "104 Wha guitar", "105 Hawaian", "106 Mandolin", "107 Django", "108 ClassicFx", "109 Pedalsteel", "110 Rock&Roll", "111 Reed 8’", "112 Fisa 8’", "113 Gategroup St", "114 Amazing St", "115 Housesynth St", "116 Resonance", "117 Key Lead St", "118 Juno Synth St", "119 Obx Poly St", "120 JX Brass", "121 Alarm&Sirene", "122 Jet", "123 Pff&Jew_Harp", "124 Rain", "125 Vocals", "126 Thunder", "127 Train", "128 Sweep"},
    {"1 Piano&Strings St, M", "2 Grand&Pad St, M", "3 Octapiano St", "4 Electropad M", "5 Msk", "6 StagePno", "7 Rodes", "8 Elopiano", "9 FM Layer", "10 Chinese Bell", "11 Stereochrom", "12 Bright Vibes", "13 Toy Box", "14 Mbira", "15 XMas Bell", "16 FM Marimba", "17 Pop Organ", "18 Gospel", "19 Power Slow", "20 Principale", "21 Dry Organ", "22 Alpen", "23 Bellow", "24 Steirisch", "25 DynaFolk", "26 CountryFx", "27 Humbuck", "28 StratoFx", "29 Ac. Stereo St", "30 Acid", "31 Plucked", "32 Flamenco", "33 Acoustic Bass", "34 Bass&Guit1", "35 Oberbas1", "36 RetroBass", "37 Fusion", "38 Oberbas2", "39 Pedalbass", "40 Bass&Guit 2", "41 Slow Violin", "42 Electric Violin", "43 Chamber", "44 Tzivago", "45 Staccato St, M", "46 String Fall St", "47 Harp&Orchestra St, M", "48 Symphony St, M", "49 Full Ensemble St, M", "50 Digistring", "51 Disco Strings St, M", "52 Country Violin", "53 Tuuh", "54 Ooh Girls", "55 Aah Girls St", "56 Choir", "57 Tbones&Cornet St, M", "58 Slide Trombone", "59 Dixie Tpt", "60 Open Horn St", "61 Brass Fall Off", "62 Horn Pad St, M", "63 Multi Brass St, M", "64 Brass Cool St, M", "65 Altosax", "66 Jazz Alto", "67 Alto Straight", "68 Rock Sax", "69 Superhorn St, M", "70 Tpt Slide Up St", "71 Dry Trumpet", "72 Trumpet Section M", "73 Open Trombones St, M", "74 Irish Flute", "75 Staccato Flute", "76 Asian Flute", "77 Fife", "78 Ethnic Flute", "79 Staccato Whistle", "80 Pipe", "81 Solo Dance St", "82 Juicy", "83 Drama", "84 Odissey", "85 Minimug M", "86 Attack St, M", "87 Phantom M", "88 Soloist St, M", "89 Warmness", "90 Explorer", "91 Choral M", "92 Cyber St, M", "93 Magic M", "94 Glockpad", "95 Stream", "96 Digi Pro", "97 UsPad", "98 Bottled Air", "99 Bellvox", "100 DigiPad St", "101 Syntax St", "102 Bright Analog St, M", "103 Aura Pad M", "104 Acoustic Dream St, M", "105 Tres", "106 Banjo 5 String", "107 Futozao", "108 Hackbrett", "109 Paraguayan Harp", "110 Short Mandolin", "111 Harp Glissando", "112 Bouzuki", "113 Tinkles", "114 Triangle", "115 Tambora", "116 Claves", "117 Ac.Tom", "118 Brushtom", "119 Snare", "120 Crash", "121 Scratch1", "122 Scratch2", "123 Scratch3", "124 Scratch4", "125 Scratch5", "126 FxSlap", "127 Laser", "128 Wind"},
    {"1 Layered Piano M", "2 Latin Piano St, M", "3 Fm Thin", "4 Rdx Operator M", "5 Twin Electro M", "6 Stage Mix M", "7 Huge FM M", "8 Digitine St, M", "9 Glasshouse St, M", "10 Fantasyland St, M", "11 Italian Positive", "12 Principale&Octave M", "13 Full Organ M", "14 ChorusB3", "15 Hot Organ", "16 16-8-5", "17 Pipe Octave", "18 Tonewheel M", "19 Movie Organ M", "20 Fast Drawbars M", "21 Carousel Org M", "22 Diatonic M", "23 Blues Harp", "24 Conservatory M", "25 Classic Bright", "26 Thin Folk", "27 Jazz Octave M", "28 Muted Wha", "29 Rock Strat", "30 Str Power", "31 Valve Lead M", "32 Smooth ElectroM", "33 Strat&Folk M", "34 Open Country St, M", "35 Golden Steel M", "36 Cntry Detune", "37 Acoust. Picking M", "38 Twin Folk M", "39 Gipsy", "40 Brazilian", "41 Rockabilly", "42 Funk Bass", "43 Maxipick", "44 Ooh Mix St", "45 Duh Group M", "46 Dah Quartet M", "47 Lah Trio M", "48 Vocal_Praise M", "49 Cinema Strings", "50 Score M", "51 AahMix St", "52 Humming", "53 Lah Choir St, M", "54 Lah Mello Choir M", "55 Church Choir M", "56 Night Vocals", "57 Mmh", "58 Boom&Uap", "59 Horn Crescendo M", "60 Emperor Brass St, M", "61 Dry Tenor", "62 Sax Club", "63 Swing Tenor", "64 Baritone", "65 Glenn Sax M", "66 Boogie Sax M", "67 Octasax M", "68 Countie Sax M", "69 Sax Section M", "70 Saxes M", "71 Billy Sax M", "72 Pop Sax M", "73 Lute", "74 Mandola", "75 Latin Harp", "76 Portuguese", "77 Fingered&Slide", "78 Fingered Gtr", "79 Finger Slap", "80 Planet", "81 Rock Synth M", "82 Chuck St, M", "83 Smooth Lead St, M", "84 Synth Plug St, M", "85 Subbass", "86 Synvoice", "87 Deep Jazz", "88 Memory Synth", "89 Evolution", "90 Sideral St, M", "91 North Wire St, M", "92 Blowing St, M", "93 Aural St, M", "94 Velvet St, M", "95 Etnovox St, M", "96 Whisper M", "97 Evopad St, M", "98 Plucking St, M", "99 Mallet Area St, M", "100 80’s Pad St, M", "101 Fluty Pad St, M", "102 Nylon Theme St, M", "103 BottledChoir", "104 Brass Hits St, M", "105 Southern Harp M", "106 Ungarian Zither M", "107 Alpen Choir M", "108 Mallet World M", "109 Country&WesternM", "110 StereoFolk St, M", "111 Alpen Musette", "112 Jazz Accordion", "113 Bright Musette M", "114 335 Jazz", "115 Smooth_Bass", "116 Funky Muted", "117 Country Gtr", "118 Nylon_slide", "119 Power Guit", "120 Clean Chorus FX", "121 Tremolo Mark FX", "122 Phase Electro FX", "123 Organ Overdrive FX", "124 Metal FX", "125 Distorted FX", "126 Crunch FX", "127 Jazz Combo St, M, FX", "128 Echolead St, FX" }
} };

ImVec2 glfwWindowSize = ImVec2(400, 500);

int currentBank = 0;
std::array<int, 4> currentBankInstrument{
    0, 0, 0, 0
};
std::array<float, 4> currentBankScroll{
    0, 0, 0, 0
};

RtMidiIn* midiIn = nullptr;
RtMidiOut* midiOut = nullptr;
int midiInPortsCount = 0;
int midiOutPortsCount = 0;
int midiOutCurrentPort = -1;

void setupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}

void setupRtMidi()
{
    try {
        midiIn = new RtMidiIn();
    }
    catch (RtMidiError& error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
    midiInPortsCount = midiIn->getPortCount();

    try {
        midiOut = new RtMidiOut(RtMidi::Api::WINDOWS_MM);
    }
    catch (RtMidiError& error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
    auto api = midiOut->getCurrentApi();
    midiOutPortsCount = midiOut->getPortCount();
}

void sendBankInstrumentToCurrentMidiOut()
{
    if (midiOutCurrentPort < 0) return;
    if (midiOut->isPortOpen() == false) return;

    unsigned char message[3] = { 0, 0, 0 };
    
    message[0] = 0xB0;
    message[1] = 0x0;
    message[2] = std::get<1>(BANK_CC[currentBank]);
    midiOut->sendMessage(message, 3);

    message[0] = 0xC0;
    message[1] = (currentBankInstrument[currentBank]);
    midiOut->sendMessage(message, 2);
}

void drawMidiDeviceSelect()
{
    ImGui::SetNextWindowSize(ImVec2(glfwWindowSize.x, 75), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::Begin("MIDI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    ImGui::PushItemWidth(ImGui::GetWindowWidth() - 20);
    std::string selectedPortName = midiOutCurrentPort == -1 ? "Not connected" : midiOut->getPortName(midiOutCurrentPort);
    ImGui::Text("MIDI Out Port");
    if (ImGui::BeginCombo("", selectedPortName.c_str(), ImGuiComboFlags_PopupAlignLeft)) {
        {
            const bool selected = -1 == midiOutCurrentPort;
            if (ImGui::Selectable("Not connected", selected)) {
                midiOutCurrentPort = -1;
                if (midiOut->isPortOpen()) {
                    midiOut->closePort();
                }
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        for (int i = 0; i < midiOutPortsCount; ++i)
        {
            const bool selected = i == midiOutCurrentPort;
            if (ImGui::Selectable(midiOut->getPortName(i).c_str(), selected))
            {
                midiOutCurrentPort = i;
                if (midiOut->isPortOpen()) {
                    midiOut->closePort();
                }
                midiOut->openPort(midiOutCurrentPort);
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void drawKetronInstrumentSelect()
{
    ImGui::SetNextWindowSize(ImVec2(glfwWindowSize.x, glfwWindowSize.y-75), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 75), ImGuiCond_Always);
    ImGui::Begin("Ketron SD1000", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    ImGui::BeginChild("bank", ImVec2(50, 20), false);
    ImGui::Text("BANK:");
    ImGui::EndChild();
    ImGui::SameLine();

    bool scrollToActiveInstrument = false;
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    for (int i = 0; i < BANK_CC.size(); ++i)
    {
        const bool selected = i == currentBank;
        if (ImGui::Selectable(std::get<0>(BANK_CC[i]).c_str(), selected, ImGuiSelectableFlags_None, ImVec2((ImGui::GetWindowWidth() - 50)/4 - 15, 20))) {
            if (currentBank != i) {
                scrollToActiveInstrument = true;
            }
            currentBank = i;
            sendBankInstrumentToCurrentMidiOut();
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
    ImGui::PopStyleVar(1);

    ImGui::BeginChild("instruments", ImVec2(0,0), true, ImGuiWindowFlags_AlwaysAutoResize);
    for (int i = 0; i < INSTRUMENT_PC[currentBank].size(); ++i)
    {
        const bool selected = i == currentBankInstrument[currentBank];
        if (ImGui::Selectable(INSTRUMENT_PC[currentBank][i].c_str(), selected)){
            currentBankInstrument[currentBank] = i;
            sendBankInstrumentToCurrentMidiOut();
        }

        if (selected && scrollToActiveInstrument)
        {
            ImGui::SetScrollHere();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void glfw_resize_callback(GLFWwindow* window, int width, int height)
{
    glfwWindowSize.x = width;
    glfwWindowSize.y = height;
}

#if (WIN32)
int WINAPI WinMain(
    HINSTANCE hInstance,     /* [input] handle to current instance */
    HINSTANCE hPrevInstance, /* [input] handle to previous instance */
    LPSTR lpCmdLine,         /* [input] pointer to command line */
    int nCmdShow             /* [input] show state of window */
)
#else
int main(int, char**)
#endif
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(glfwWindowSize.x, glfwWindowSize.y, "Ketron SD1000 Instrument Selector by aksyr", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwSetWindowSizeLimits(window, 200, 250, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSizeCallback(window, glfw_resize_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsLight();
    setupImGuiStyle();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    setupRtMidi();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        drawMidiDeviceSelect();
        drawKetronInstrumentSelect();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
