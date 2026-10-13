
// Program na plytkę "Rapsberry" dla eksponatu Van de Graffa
// #define HARDW 1
#define HARDW 0

// Docelowo nie będziemy pisac do Serial port
#define NO_SERIAL 0

#include <iostream>
#include <list>
#include <vector>
#include <cstdlib>
#include <format>


#if HARDW
// #include <dummy_rp2040.h>
#else
#include <chrono>
#include <thread>

#include <cstdio>
#ifdef _WIN32
#include <windows.h>  // dla polskich liter
#endif
#endif


using namespace std;

#if HARDW
#include <FastLED.h>
#define PIN_BUTTON_POLSKI 17  // Pin przycisku startowego (z rezystorem PULLUP)
#define PIN_BUTTON_ANGIELSKI 17

#define PIN_STATUS_LED LED_BUILTIN  // Dioda stanu na płytce
// ==========================================
// PINY SYGNAŁOWE ARDUINO
// ==========================================
#define PIN_AUDIO_BASE 2
#define PIN_LED_1 10
#define PIN_LED_2 11
#define PIN_LED_3 12
#define PIN_LED_4 13
#define PIN_LED_5 14
#define PIN_LED_6 15
#define PIN_LED_7 16
#define PIN_BELT 18


#define NUM_LEDS_1 130  // Taśma 1: Rura akceleracyjna / Jonowód
#define NUM_LEDS_2 110  // Taśma 2: Pozioma taśma VdG / / Dzielnik
#define NUM_LEDS_3 41   // Taśma 3: Prawa listwa / Grzebień odbiorczy
#define NUM_LEDS_4 41   // Taśma 4: Lewa listwa / Grzebień oddający
#define NUM_LEDS_5 10   // Taśma 5: Czasza   ?????????????????
#define NUM_LEDS_6 75   // Taśma 6: Przepływ gazu / Źródło jonów
#define NUM_LEDS_7 492  // Taśma 7: Elektrody (12 segmentów x 41 LED)

// ==========================================
// KONFIGURACJA PASA TRANSMISYJNEGO
// ==========================================
#define NUM_LEDS_PER_STRIP 134
#define NUM_BELT_STRIPS 6
#define NUM_LEDS_BELT_TOTAL (NUM_LEDS_PER_STRIP * NUM_BELT_STRIPS)

#endif

bool flag_angielski = false;

////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
struct Tkolor {
    short int r;
    short int g;
    short int b;
};
//////////////////////////////////////////////////////
constexpr Tkolor czarny{ 0, 0, 0 };
constexpr Tkolor bialy{ 255, 255, 255 };
constexpr Tkolor czerwony{ 255, 0, 0 };
constexpr Tkolor niebieski{ 0, 0, 255 };
constexpr Tkolor zielony{ 0, 255, 0 };
constexpr Tkolor zolty{ 255, 255, 0 };
constexpr Tkolor pomaranczowy{ 255, 128, 0 };
constexpr Tkolor cyjan{ 0, 255, 255 };

//////////////////////////////////////////////////////////
class Telement_animowany {
public:
    string nazwa;
    virtual ~Telement_animowany() = default;

    virtual void kolejna_klatka() = 0;
    virtual void update_diody_bez_wyslania() = 0;
    // void ustaw_nazwe(string n) {
    //     nazwa = n;
    // }
    virtual void reset(){};
};
//*************************************************************
void czekaj_milisekund(int ile_milisekund) {
#if HARDW
    delay(ile_milisekund);
#else
    this_thread::sleep_for(chrono::milliseconds(ile_milisekund));
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void wykonaj_test();  // deklaracje funkcji
void wykonaj_czesc_pierwsza();
void wykonaj_czesc_druga();
///////////////////////////////////////////////////////////////
struct TLED {
    bool on = false;
    Tkolor moj_kolor = czarny;
    int intensywnosc = 255;  // czyli full
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The Hardware_LED class
#if HARDW
class Hardware_LED {
private:
    CRGB leds1[NUM_LEDS_1];
    CRGB leds2[NUM_LEDS_2];
    CRGB leds3[NUM_LEDS_3];
    CRGB leds4[NUM_LEDS_4];
    CRGB leds5[NUM_LEDS_5];
    CRGB leds6[NUM_LEDS_6];
    CRGB leds7[NUM_LEDS_7];
    CRGB leds_belt[NUM_LEDS_BELT_TOTAL];

public:

    void inicjalizuj();

    void ustaw_led(
        int kanal,
        int nr_led,
        const TLED& led);

    void wyslij() {
        FastLED.show();
    }

    void tecza(int kan_fiz, int ile) {
        switch (kan_fiz) {
        case 10: fill_rainbow(leds1, ile); break;
        case 11: fill_rainbow(leds2, ile); break;
        case 12: fill_rainbow(leds3, ile); break;
        case 13: fill_rainbow(leds4, ile); break;
        case 14: fill_rainbow(leds5, ile); break;
        case 15: fill_rainbow(leds6, ile); break;
        case 16: fill_rainbow(leds7, ile); break;

        case 18: fill_rainbow(leds_belt, ile); break;
        }
    }

    void wyczysc();
    //--------------------------------------------------------------------------
    void clearAll() {
        wyczysc();
    }
};
//****************************************************************************
void Hardware_LED::wyczysc() {
    fill_solid(leds1, NUM_LEDS_1, CRGB::Black);
    fill_solid(leds2, NUM_LEDS_2, CRGB::Black);
    fill_solid(leds3, NUM_LEDS_3, CRGB::Black);
    fill_solid(leds4, NUM_LEDS_4, CRGB::Black);
    fill_solid(leds5, NUM_LEDS_5, CRGB::Black);
    fill_solid(leds6, NUM_LEDS_6, CRGB::Black);
    fill_solid(leds7, NUM_LEDS_7, CRGB::Black);
    fill_solid(leds_belt, NUM_LEDS_BELT_TOTAL, CRGB::Black);

    FastLED.show();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Hardware_LED::inicjalizuj() {
    FastLED.addLeds<WS2812B, 10, GRB>(leds1, NUM_LEDS_1);
    FastLED.addLeds<WS2812B, 11, GRB>(leds2, NUM_LEDS_2);
    FastLED.addLeds<WS2812B, 12, GRB>(leds3, NUM_LEDS_3);
    FastLED.addLeds<WS2812B, 13, GRB>(leds4, NUM_LEDS_4);
    FastLED.addLeds<WS2812B, 14, GRB>(leds5, NUM_LEDS_5);
    FastLED.addLeds<WS2812B, 15, GRB>(leds6, NUM_LEDS_6);
    FastLED.addLeds<WS2812B, 16, GRB>(leds7, NUM_LEDS_7);
    FastLED.addLeds<WS2812B, 18, GRB>(leds_belt, NUM_LEDS_BELT_TOTAL);

    FastLED.setBrightness(255);

#define PIN_AUDIO_BUSY 20
    pinMode(PIN_AUDIO_BUSY, INPUT);

    wyslij();
}
//*********************************************************************
void Hardware_LED::ustaw_led(
    int kanal,
    int nr_led,
    const TLED& led) {
    CRGB kolor = CRGB::Black;

    if (led.on) {
        kolor.r = led.moj_kolor.r;
        kolor.g = led.moj_kolor.g;
        kolor.b = led.moj_kolor.b;

        kolor.nscale8(
            led.intensywnosc);
    }

    // sprawdzenie czy to w legalnym zakresie

    switch (kanal) {
    case 10:
        if (nr_led >= 0 && nr_led < NUM_LEDS_1)
            leds1[nr_led] = kolor;
        break;

    case 11:
        if (nr_led >= 0 && nr_led < NUM_LEDS_2)
            leds2[nr_led] = kolor;
        break;

    case 12:
        if (nr_led >= 0 && nr_led < NUM_LEDS_3)
            leds3[nr_led] = kolor;
        break;

    case 13:
        if (nr_led >= 0 && nr_led < NUM_LEDS_4)
            leds4[nr_led] = kolor;
        break;

    case 14:
        if (nr_led >= 0 && nr_led < NUM_LEDS_5)
            leds5[nr_led] = kolor;
        break;

    case 15:
        if (nr_led >= 0 && nr_led < NUM_LEDS_6)
            leds6[nr_led] = kolor;
        break;

    case 16:
        if (nr_led >= 0 && nr_led < NUM_LEDS_7)
            leds7[nr_led] = kolor;
        break;


        //  case 17 to jest dzwiek

    case 18:
        if (nr_led >= 0 && nr_led < NUM_LEDS_BELT_TOTAL)
            leds_belt[nr_led] = kolor;
        break;
    }
}
//********************************************************
#if NO_SERIAL

struct TSerial {
    void print(string txt) {}
    void println(string txt) {}
    void println(int n) {}
    void print(int n) {}
    void printf(const char* txt, int n) {}
    void begin(int baud) {}
} S;
#else
auto& S = Serial;  // alias obiektu
#endif

#else  // //////////////////// Gdy komputer
class Hardware_LED {
public:
    void wyslij() {
        // S.println("Wyslanie o hardware'u " );
    }
};
//////////////////////////////////////////'

struct TSerial {
    void print(string txt);
    void println(string txt);
    void println(int n);
    void print(int n);
    void printf(const char* txt, int n) {
        print(format(" {} {}", txt, n));
    }
} S;
//*************************************************************************
void TSerial::print(string txt) {
    cout << txt;
}
//*************************************************************************
void TSerial::println(string txt) {
    cout << txt << endl;
}
//*************************************************************************
void TSerial::println(int n) {

    cout << n << endl;
}
//*************************************************************************
void TSerial::print(int n) {
    cout << n;
}
//*************************************************************************
// TSerial Serial;

#endif




//--------------------------------------------------------
//*************************************************
// Hardware_LED hardware_LED;
Hardware_LED H;
//////////////////////////////////////////////////////////
class Tzespol_diod : public Telement_animowany {
protected:
public:
    int nr_kanalu = -1;
    int nr_poczatkowej_diody;
    int nr_koncowej_diody;
    int liczba_ledow_obiektu = 0;

    std::vector<TLED> Led;

public:

    Tzespol_diod(int kan, int pocz, int kon)
        : nr_kanalu(kan), nr_poczatkowej_diody(pocz), nr_koncowej_diody(kon) {
        liczba_ledow_obiektu = kon - pocz + 1;
        Led.resize(liczba_ledow_obiektu);
    }

    //--------------------------------------------------------------
    Tzespol_diod()
        : nr_kanalu(0), nr_poczatkowej_diody(0), nr_koncowej_diody(0) {
        // S.println("Uwaga, uzyty konstruktor domniemany! "  );
        liczba_ledow_obiektu = nr_koncowej_diody - nr_poczatkowej_diody + 1;
        Led.resize(liczba_ledow_obiektu);
    }
    //--------------------------------------------------------------

    void info() {
        S.print("ABC");
        S.print(format(" {} kanal= {} , led pocz = {}, led _kon= {}, liczba_ledow_listwy {} \n",
                       nazwa, nr_kanalu,
                       nr_poczatkowej_diody, nr_koncowej_diody,
                       liczba_ledow_obiektu)
                    .c_str());
    }
    // void tecza() {
    //   int nr_fizyczny = kanal_fizyczny[nr_kanalu];
    //   H.tecza(nr_fizyczny, liczba_ledow_listwy);
    //   H.wyslij();
    // }

    virtual ~Tzespol_diod() = default;

    //--------------------------------------------------------------
    // Oblicza następną klatkę animacji.
    virtual void kolejna_klatka() {}

    //--------------------------------------------------------------
    static constexpr int kanal_fizyczny[] = {
        -1,  // 0 - nieużywany
        10,  // 1
        11,  // 2
        12,  // 3
        13,  // 4
        14,  // 5
        15,  // 6
        16,  // 7
        18,  // 8
        11   // 9 - czasza, ta sama taśma co dzielnik
    };

    //--------------------------------------------------------------
    // Wysyła aktualny stan Led[] do klasy hardware_LED.
    void update_diody_bez_wyslania() {
#if HARDW
        int nr_fizyczny = kanal_fizyczny[nr_kanalu];

        for (int i = 0; i < liczba_ledow_obiektu; ++i) {
            int nr_led_fizycznego =
                nr_poczatkowej_diody + i;

            H.ustaw_led(
                nr_fizyczny,
                nr_led_fizycznego,
                Led[i]);
        }
#else
        wypisz_tresc();
#endif
    }

    //*************************************************************
    void ustaw_parametry(int kan, int pocz, int kon) {
        nr_kanalu = kan;
        nr_poczatkowej_diody = pocz;
        nr_koncowej_diody = kon;
        liczba_ledow_obiektu = kon - pocz + 1;
        Led.resize(liczba_ledow_obiektu);
    }
    //*************************************************************
    //*************************************************************
    virtual void zamigaj_i_update(Tkolor kolor,
                                  int milisekund,
                                  double ile_sekund,
                                  bool flag_zostawic_swiecace) {
        S.println(("Miga " + nazwa).c_str());

        bool ON_off = true;

#if HARDW

        unsigned long start = millis();

        while (true) {
            unsigned long teraz = millis();
            unsigned long t = teraz - start;

            // S.print(format("tejtej animacji = {} \n", t / 1000.0).c_str());

            if (t >= (unsigned long)(ile_sekund * 1000.0))
                break;

            if (ON_off)
                zapal_wszystkie(kolor);
            // tecza();
            else
                zgas_wszystkie();

            ON_off = !ON_off;

            update_diody_bez_wyslania();
            H.wyslij();

            czekaj_milisekund(milisekund);
        }

#else

        auto start = chrono::steady_clock::now();

        while (true) {
            auto teraz = chrono::steady_clock::now();

            double t =
                chrono::duration<double>(teraz - start).count();

            // S.print(format("Czas tej animacji = {} ",  t ) );

            if (t >= ile_sekund)
                break;

            if (ON_off)
                zapal_wszystkie(kolor);
            else
                zgas_wszystkie();

            ON_off = !ON_off;

            update_diody_bez_wyslania();

            czekaj_milisekund(milisekund);
        }

#endif

        if (flag_zostawic_swiecace)
            zapal_wszystkie(kolor);
        else
            zgas_wszystkie();

        update_diody_bez_wyslania();
        H.wyslij();
    }
    //*************************************************************
    void zapal_wszystkie(Tkolor kol) {
        for (int i = 0; i < liczba_ledow_obiektu; ++i) {
            Led[i].on = true;
            Led[i].moj_kolor = kol;
            Led[i].intensywnosc = 255;
        }
        update_diody_bez_wyslania();
        H.wyslij();
    }
    //*************************************
    void zapal_wszystkie(Tkolor kol, int intens) {
        for (int i = 0; i < liczba_ledow_obiektu; ++i) {
            Led[i].on = true;
            Led[i].moj_kolor = kol;
            Led[i].intensywnosc = (intens < 255) ? intens : 255;
        }
        update_diody_bez_wyslania();
        H.wyslij();
    }
    //*************************************
    void zgas_wszystkie() {
        for (int i = 0; i < liczba_ledow_obiektu; ++i) Led[i].on = false;
        update_diody_bez_wyslania();
    }
    //*************************************
    void zapal_led_nr(int nr, Tkolor kol, int poprzednich = 0) {

        for (int p = 0; p < poprzednich + 1; ++p) {
            auto np = nr - p;
            if (np >= 0 && np < liczba_ledow_obiektu) {
                Led[np].on = true;
                Led[np].moj_kolor = kol;
            }
        }
        update_diody_bez_wyslania();
    }
    //*************************************
    void zapal_led_nr_intensywnosc(int nr, Tkolor kol, int intens) {

        if (nr >= 0 && nr < liczba_ledow_obiektu) {
            Led[nr].on = true;
            Led[nr].moj_kolor = kol;
            Led[nr].intensywnosc = intens;
        }
        update_diody_bez_wyslania();
    }
    //*******************************************
    virtual void wypisz_tresc() {
        // S.println(__PRETTY_FUNCTION__ );
        string tresc;
        tresc.append(nazwa);
        for (int p = 0; p < liczba_ledow_obiektu; ++p) {
            if (Led[p].on) {
                tresc.append("o");
            } else {
                tresc.append("_");
            }
        }

        S.println(tresc.c_str());
        // S.print("\n");
    }
};
/////////////////////////////////////////////////////////////////////////////////////
class Tlistwa_nakladajaca_ladunki : public Tzespol_diod {
public:
    Tlistwa_nakladajaca_ladunki(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
    }
};
//////////////////////////////////////////////////////////////////////////////////////
class Tlistwa_odbierajaca : public Tzespol_diod {
    // string poprzednia_tresc;

    double factor;
public:

    Tlistwa_odbierajaca(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
        factor = double(liczba_ledow_obiektu) / 6;
    }

    //-------------------------------------------------------------------------------------------------------

    void iskrzy_klatka(const string& przekroj, bool flag_wstepnie_zamigaj) {
        static int ile_bylo_migniec = 15;

        zgas_wszystkie();

        if (flag_wstepnie_zamigaj && ile_bylo_migniec > 0) {
            {
                static bool on_off = 0;
                // poczatkowe miganie na bialo
                if (on_off) zapal_wszystkie(bialy);
                else zgas_wszystkie();
                on_off = !on_off;
                --ile_bylo_migniec;

                // S.println("Iskrzenie " <<  endl;
            }
        } else
            // Każdy '*' w przekroju reprezentuje odebrany ładunek.
            for (int r = 0; r < (int)przekroj.size(); ++r) {
                if (przekroj[r] == '*') {
                    // jeśli jest tylko 6 ledow w listwie, to mozemy
                    // robić to bezposrednio
                    //zapal_led_nr(r, czerwony);
                    // rozszerzenie tego na 40 ledow listwy
                    for (int i = 0; i < (int)factor; ++i) {

                        int n = ((r * (factor - 2)) + i) + (factor / 2) + 3;

                        zapal_led_nr(n, czerwony);
                    }
                }
            }

#if HARDW == 0
        wypisz_tresc();


#endif
    }
};

//////////////////////////////////////////////////////
class Tlistwa_pasa : public Tzespol_diod {
public:
};
//////////////////////////////////////////////////////
class Tczasza : public Tzespol_diod {
    int aktualna_intensywnosc = 0;
    int przyrost_intensywnosci = 20;

public:

    Tczasza(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;

        for (int i = 0; i < liczba_ledow_obiektu; ++i)

            Led[i].intensywnosc = 0;
    }

    //************************************************************
    void ustaw_przyrost_intensywnosci(int przyrost) {
        przyrost_intensywnosci = przyrost;
    }
    //************************************************************
    void kolejna_klatka() override {

        // static int klatka_nr = 0 ;
        // if((++klatka_nr % 10) != 0 ) return;

        // if (aktualna_intensywnosc >= 255)
        //   return;  // false;

        // aktualna_intensywnosc += przyrost_intensywnosci;

        // if (aktualna_intensywnosc > 255)
        // aktualna_intensywnosc = 255;


        int rd = rand() % liczba_ledow_obiektu;
        int j = Led[rd].intensywnosc;
        // S.printf("intensywnosc ledu nr %d jest %d", rd, j );

        j += przyrost_intensywnosci;
        if (j >= 255) j = 253;
        zapal_led_nr_intensywnosc(rd, czerwony, ++j);

        return;
    }
    //---------------------------------------------------------------------
    void wypisz_tresc() override {
        S.print(format("{}  intensywnosc = {} \n", nazwa, (Led[0].on ? Led[0].intensywnosc : 0)).c_str());
    }

    //************************************************************
    void wyzeruj_rozjarzanie() {
        aktualna_intensywnosc = 0;
        zgas_wszystkie();
    }

    //************************************************************
    void zapal_od_razu() {
        aktualna_intensywnosc = 255;
        zapal_wszystkie(czerwony, 255);
    }
};
//////////////////////////////////////////////////////////////////////////////////////////
class Tbutla : public Tzespol_diod {
public:
    Tbutla(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
    }

    //--------------------------------
    void wypisz_tresc() {
        if (Led[0].on)
            S.println(format(" {}  ON, intensywnosc = {} \n", nazwa, Led[0].intensywnosc).c_str());
    }
};
//////////////////////////////////////////////////////////////////////////////////////////
class Tzrodlo_jonow : public Tzespol_diod {
    int nr_klatki = 0;
    bool on_off = true;
public:
    Tzrodlo_jonow(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
    }
    //--------------------------------------------------------------
    void reset() {
        nr_klatki = 0;
        on_off = true;
    }
    //--------------------------------------------------------------
    void kolejna_klatka() {
        nr_klatki++;

        if (nr_klatki < 50) {

            if (nr_klatki % 10 == 0)
                on_off = !on_off;

            if (on_off) zapal_wszystkie(pomaranczowy, 255);
            else zgas_wszystkie();
        } else {
            zapal_wszystkie(pomaranczowy, 255);
        }
    }
    //*********************************************************************************
    void wypisz_tresc() {
        S.print(format("  Zrodlo {} \n", (Led[0].on ? "ON" : "off")).c_str());
    }
};
//////////////////////////////////////////////////////////////////////////////////////////
class Telektroda : public Tzespol_diod {
    int elektroda_nr;
    static int ile_elektrod;
    // Tkolor kolor;
public:
    Telektroda()
        : Tzespol_diod() {
        int ile_ledow_w_elektrodzie = 41;
        elektroda_nr = ile_elektrod;
        int kanal_poczatkowy = 7;
        int p = (ile_ledow_w_elektrodzie)*elektroda_nr;
        int k = p + ile_ledow_w_elektrodzie - 1;
        ustaw_parametry(kanal_poczatkowy, p, k);
        nazwa = "elektroda nr " + to_string(elektroda_nr);

        ++ile_elektrod;
    }

    //--------------------------------------------------------------
    void wypisz_tresc() {
        S.print(
            format("Elektroda nr {} ON = {} ,kolor = R G B [{:x} {:x} {:x}] \n",
                   elektroda_nr,
                   Led[0].on,
                   Led[0].moj_kolor.r,
                   Led[0].moj_kolor.g,
                   Led[0].moj_kolor.b)
                .c_str());
        for (int i = 0; i < liczba_ledow_obiektu; ++i) {
            S.print(Led[i].on ? "o" : "_");
        }
        S.println("]");
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tpas : public Telement_animowany {
public:

    // Liczba fizycznych LED-ów w jednym rzędzie.

    static constexpr int ile_ledow_w_rzedzie = 133;

    // sześć listew LED-ów
    static constexpr int ile_rzedow = 6;
    Tlistwa_pasa rzad[ile_rzedow];
    int intens = 255;

    enum class Tryb_animacji {
        brak,
        strzalki,
        ladunki
    };

    Tryb_animacji tryb_animacji = Tryb_animacji::brak;

    std::string poprzednia_tresc_pasa;
    std::vector<string> pattern_ladunkow;

    double pozycja_ladunkow = 0.0;
    double pozycja_strzalek = 0.0;

    // bool animacja_ladunkow_rozpoczeta = false;
    // int gestosc_ladunkow = 5;

    bool animuj_listwe_odbierajaca = false;
    Tlistwa_odbierajaca* wsk_listwy_odbierajacej = nullptr;

    bool wstepne_miganie_listwy = false;
    //******************************************
    // konstruktor
    //******************************************
    Tpas(int nr_kanalu) {
        for (int i = 0; i < ile_rzedow; ++i) {
            int p = (ile_ledow_w_rzedzie + 1) * i;
            int k = p + ile_ledow_w_rzedzie;
            rzad[i].ustaw_parametry(nr_kanalu, p, k);
        }
    }
    //--------------------------------------------------------------
    void reset() override {
        pozycja_ladunkow = 0.0;
        pozycja_strzalek = 0.0;
        tryb_animacji = Tryb_animacji::brak;
    }
    //******************************************************************
    void start_animacji_ladunkow(
        int gestosc,
        Tlistwa_odbierajaca* wsk_listwy) {
        pattern_ladunkow = generuj_random_pattern(gestosc);
        //pozycja_ladunkow = 0;
        wsk_listwy_odbierajacej = wsk_listwy;
    }
    //******************************************************************
    void ustaw_animacje_ladunkow(
        bool animuj_listwe_odbier,
        bool wstepne_miganie) {
        animuj_listwe_odbierajaca = animuj_listwe_odbier;
        wstepne_miganie_listwy = wstepne_miganie;

        tryb_animacji = Tryb_animacji::ladunki;
    }
    //******************************************************************
    void start_animacji_strzalek() {
        pozycja_strzalek = 0;
        tryb_animacji = Tryb_animacji::strzalki;
    }
    //******************************************************************
    //******************************************************************
    void kolejna_klatka() override {
        switch (tryb_animacji) {
        case Tryb_animacji::strzalki:
            kolejna_klatka_strzalek();
            break;

        case Tryb_animacji::ladunki:
            kolejna_klatka_ladunkow();
            break;

        case Tryb_animacji::brak:
            break;
        }
    }
    //******************************************************************
    void kolejna_klatka_ladunkow() {
        if (pattern_ladunkow.empty())
            return;

        // zwolnienie ruchu ładunków
        static int licznik = 0 ;
        ++licznik;
        licznik %= 3;
        if(licznik != 0)return;



        const int dlugosc_wzoru =
            pattern_ladunkow.size();

        // ------------------------------------------------------------
        // Przesuwamy ładunki o jeden LED w prawo.
        // ------------------------------------------------------------

        ++pozycja_ladunkow;
        zerowanie_ledow();


        // ------------------------------------------------------------
        // Ile LED-ów pasa zostało już osiągniętych przez ładunki?
        //
        // Na początku:
        //
        //     pozycja_ladunkow = 1
        //
        // więc aktywny jest tylko LED 0 i LED 1.
        //
        // Gdy pozycja osiągnie 109, cały pas jest już zapełniony.
        // ------------------------------------------------------------

        int ostatni_dostepny_led =
            static_cast<int>(pozycja_ladunkow);

        if (ostatni_dostepny_led >= ile_ledow_w_rzedzie)
            ostatni_dostepny_led =
                ile_ledow_w_rzedzie - 1;


        // ------------------------------------------------------------
        // Budujemy obraz sześciu rzędów pasa.
        //
        // LED 0                 -> lewy koniec
        // LED 109               -> prawy koniec
        //
        // Wzór przesuwa się z LEWEJ do PRAWEJ.
        // ------------------------------------------------------------

        for (int r = 0;
             r < ile_rzedow;
             ++r) {
            for (int led = 0;
                 led <= ostatni_dostepny_led;
                 ++led) {
                int indeks =
                    static_cast<int>(pozycja_ladunkow) - led;


                indeks %= dlugosc_wzoru;

                if (indeks < 0)
                    indeks += dlugosc_wzoru;


                rzad[r].Led[led].on =
                    (pattern_ladunkow[indeks][r] == '*');


                if (rzad[r].Led[led].on)
                    rzad[r].Led[led].moj_kolor = czerwony;
                rzad[r].Led[led].intensywnosc = intens; // 50;  // bylo 150
            }
        }


        // ------------------------------------------------------------
        // Prawy koniec pasa.
        //
        // Dopiero kiedy ładunki rzeczywiście dotrą do prawego końca,
        // przekazujemy pattern z ostatniego LED-a do listwy
        // odbierającej.
        // ------------------------------------------------------------

        if (pozycja_ladunkow >= ile_ledow_w_rzedzie) {
            int indeks_odebranego =
                static_cast<int>(pozycja_ladunkow)
                - (ile_ledow_w_rzedzie - 1);


            indeks_odebranego %= dlugosc_wzoru;

            if (indeks_odebranego < 0)
                indeks_odebranego += dlugosc_wzoru;


            // --------------------------------------------------------
            // Listwa odbierająca.
            // --------------------------------------------------------

            if (animuj_listwe_odbierajaca) {
                if (wsk_listwy_odbierajacej) {
                    wsk_listwy_odbierajacej->iskrzy_klatka(
                        pattern_ladunkow[indeks_odebranego],
                        wstepne_miganie_listwy);
                } else {
                    S.println("podaj wskaznik listwy odbierajcej ladunki");
                }
            }
        }
    }

    //*******************************************************************
    void kolejna_klatka_strzalek() {
        const string strzalka[3] = {
            "_____>__",
            "______>>",
            "_____>__"
        };

        const int dlugosc =
            strzalka[0].size();

        const int odstep = 12;

        const int liczba_strzalek =
            (ile_ledow_w_rzedzie / odstep + 20) * 5;

        zerowanie_ledow();
        // ------------------------------------------------------------
        // Przesuwamy wszystkie strzałki o jeden LED.
        // ------------------------------------------------------------
        ++pozycja_strzalek;
        // ------------------------------------------------------------
        // Rysujemy kolejne strzałki.
        // ------------------------------------------------------------
        for (int numer = 0;
             numer < liczba_strzalek;
             ++numer) {
            int poczatek = pozycja_strzalek - numer * odstep;


            for (int r = 0; r < 3; ++r) {
                for (int k = 0; k < dlugosc; ++k) {
                    if (strzalka[r][k] != '>')
                        continue;

                    int kolumna =
                        poczatek + k;

                    if (kolumna < 0 || kolumna >= ile_ledow_w_rzedzie)
                        continue;

                    // Górne trzy listwy
                    rzad[r].Led[kolumna].on = true;
                    rzad[r].Led[kolumna].moj_kolor = bialy;
                    rzad[r].Led[kolumna].intensywnosc = intens ;
                    // Dolne trzy listwy
                    rzad[r + 3].Led[kolumna].on = true;
                    rzad[r + 3].Led[kolumna].moj_kolor = bialy;
                    rzad[r + 3].Led[kolumna].intensywnosc = intens ;
                }
            }
        }
    }
    //---------------------------------------------------------------------------
    void zaswiec_na_bialo() {
        for (int r = 0;
             r < ile_rzedow;
             ++r) {
            rzad[r].zapal_wszystkie(bialy);
            rzad[r].wypisz_tresc();
        }
    }
    //---------------------------------------------------------------------------
    void zerowanie_ledow() {
        for (int rz = 0;
             rz < ile_rzedow;
             ++rz) {
            rzad[rz].zgas_wszystkie();
        }
    }
    //--------------------------------------------------------------------------
    bool wypisz_tresc_pasa() {
        string tresc;
        for (int rz = 0;
             rz < ile_rzedow;
             ++rz) {
            for (int p = 0;
                 p < ile_ledow_w_rzedzie;
                 ++p) {
                if (rzad[rz].Led[p].on)
                    tresc.append("*");
                else
                    tresc.append("_");
            }

            tresc.append("\n");
        }

        // Nic się nie zmieniło
        if (tresc == poprzednia_tresc_pasa)
            return false;

        // Obraz się zmienił
        S.print(tresc.c_str());
        poprzednia_tresc_pasa = tresc;
        return true;
    }
    //*******************************************************************************
    vector<string> generuj_random_pattern(
        int gestosc = 5) {
        vector<string> pattern;

        for (int i = 0; i < 300; ++i) {
            string wzor;
            for (int n = 0; n < 6; ++n) {
                int rd = rand();

                wzor.push_back(
                    (rd % gestosc) ? '_' : '*');
            }
            pattern.push_back(wzor);
        }
        return pattern;
    }
    //**************************************************************************************************************
    void update_diody_bez_wyslania() override {
        for (int nr = 0;
             nr < ile_rzedow;
             ++nr) {
            rzad[nr].update_diody_bez_wyslania();
        }
    }

};  // koniec klasy TPas ////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Trurka_wodoru : public Tzespol_diod {
    std::list<bool> kolejka;
    int licznik_klatek = 0;


    // Wzór przepływu wodoru:
    // jeden świecący LED, a następnie trzy wygaszone.
    bool tablica[4]{ true, false, false, false };
public:

    Trurka_wodoru(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
    }

    //***************************************************************
    void start_animacji() {
        kolejka.clear();
        licznik_klatek = 0;
        zgas_wszystkie();
    }

    //***************************************************************
    void przenies_z_kolejki_na_ledy() {
        auto it = kolejka.begin();

        for (int s = 0;
             s < liczba_ledow_obiektu;
             ++s, ++it) {
            if (it == kolejka.end())
                break;

            Led[s].on = *it;
            Led[s].moj_kolor = zielony;
        }
    }
    //***************************************************************
    void kolejna_klatka() override {
        static int goniec;
        if (licznik_klatek % 10 == 0) {
            // Dopisujemy nowy fragment przepływu
            // na początku kolejki.
            kolejka.push_front(tablica[goniec % 4]);
            goniec++;
            goniec = goniec % 4;

            // Kolejka nie może być dłuższa niż rurka.
            if ((int)kolejka.size() > liczba_ledow_obiektu)
                kolejka.pop_back();

            // Przenosimy aktualny stan kolejki
            // na LED-y.
            przenies_z_kolejki_na_ledy();
        }

        ++licznik_klatek;

        return;
    }
    //***************************************************************
    void wypisz_tresc() override {
        string tresc;

        for (int p = 0;
             p < liczba_ledow_obiektu;
             ++p) {
            if (Led[p].on)
                tresc.append("o");
            else
                tresc.append("_");
        }

        S.print(format("\rRurka wodoru: {} \n", tresc).c_str());
    }
};  ///////// KON KLASY Trurka_wodoru ////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tlistwa_dzielnika : public Tzespol_diod {
    int aktualny_led = 0;

public:

    Tlistwa_dzielnika(int kan, int pocz, int kon, string nazwa_)
        : Tzespol_diod(kan, pocz, kon) {
        nazwa = nazwa_;
    }


    //***********************************************************
    // Przygotowanie animacji.
    // Wszystkie LED-y są gaszone i zaczynamy od pierwszego.
    //
    void start_animacji() {
        zgas_wszystkie();
        aktualny_led = 0;
    }
    //***********************************************************
    // Jedna klatka animacji.
    //
    // Zapala jeden kolejny LED w jego docelowym kolorze.

    void kolejna_klatka() override {
        // Animacja zakończona – kolejne klatki nic już nie robią.
        if (aktualny_led >= liczba_ledow_obiektu)
            return;

        Led[aktualny_led].moj_kolor = daj_kolor(aktualny_led);

        Led[aktualny_led].intensywnosc = 255;
        Led[aktualny_led].on = true;

        ++aktualny_led;
        czekaj_milisekund(100);

        return;
    }
    //***********************************************************
    Tkolor daj_kolor(int nr) {
        int c = liczba_ledow_obiektu - nr;

        Tkolor kolor;

        kolor.r = int(0xff * c / (double)liczba_ledow_obiektu);
        kolor.b = int(0xff * nr / (double)liczba_ledow_obiektu);
        kolor.g = 0;

        return kolor;
    }
    //************************************************************
    void zapal_wszystkie_teczowo() {
        for (int i = 0; i < liczba_ledow_obiektu; ++i) {
            Led[i].on = true;
            Led[i].moj_kolor = daj_kolor(i);
            Led[i].intensywnosc = 255;
        }
        update_diody_bez_wyslania();
    }

    //************************************************************
    void wypisz_tresc() override {
        return;
        S.println("Dzielnik :  ");
        string trescN;
        string trescR;
        string trescG;
        string trescB;

        for (int n = 0;
             n < liczba_ledow_obiektu;
             n += 12)
        // n += 10)
        {
            auto nr = to_string(n % 50);

            trescN.append(nr);
            trescR.append(to_string(Led[n].moj_kolor.r));
            trescG.append(to_string(Led[n].moj_kolor.g));
            trescB.append(to_string(Led[n].moj_kolor.b));

            trescN.append(" ");
            trescR.append(" ");
            trescG.append(" ");
            trescB.append(" ");
        }

        cout
            << "N " << trescN << "\n"
            << "R " << trescR << "\n"
            << "G " << trescG << "\n"
            << "B " << trescB << "\n"
            << flush;
    }

    //************************************************************
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class Trura_akceleracyjna : public Tzespol_diod {
#if HARDW

    unsigned long start = millis();
#else
    std::chrono::time_point<std::chrono::steady_clock> start;
#endif
    // std::vector<int> stare_pozycje;

    enum class Tryb_animacji {
        brak,
        wolna,
        szybka,
        pojedyncza
    };


    int liczba_grup = 0;
    double czas_przejazdu = 0.0;
    int smuga = 0;
    double opoznienie = 0.0;

public:
    Tryb_animacji tryb = Tryb_animacji::brak;

    Trura_akceleracyjna(int kan, int pocz, int kon)
        : Tzespol_diod(kan, pocz, kon) {
    }

    //******************************************************
    void przyspiesz() {
        //tryb = Tryb_animacji::szybka;
        start_animacji_szybkiej();
    }
    //******************************************************
    void start_animacji_pojedynczej() {
        liczba_grup = 1;
        czas_przejazdu = 5.0;
        smuga = 1;
        opoznienie = 0.3;

        czas_startu_animacji();

        tryb = Tryb_animacji::pojedyncza;
    }

    //******************************************************
    void start_animacji_wolnej() {
        liczba_grup = 10;
        czas_przejazdu = 5.0;
        smuga = 1;
        opoznienie = 0.6;

        czas_startu_animacji();

        tryb = Tryb_animacji::wolna;
    }

    //******************************************************
    void start_animacji_szybkiej() {
        liczba_grup = 180;
        czas_przejazdu = 0.5;  // bylo
        smuga = 15;
        opoznienie = 0.5;

        czas_startu_animacji();

        tryb = Tryb_animacji::szybka;
    }
    //******************************************************
    void kolejna_klatka() override {
        switch (tryb) {
        case Tryb_animacji::brak:
            break;

        case Tryb_animacji::pojedyncza:
            klatka_animacji(
                czas_przejazdu,
                1,  // smuga
                1,  // liczba_grup,
                opoznienie);
            break;

        case Tryb_animacji::wolna:
            klatka_animacji(
                czas_przejazdu,
                smuga,
                liczba_grup,
                opoznienie);
            break;

        case Tryb_animacji::szybka:
            klatka_animacji(
                czas_przejazdu,
                smuga,
                liczba_grup,
                opoznienie);

            break;
        }
        return;
    }
    //******************************************************
    void czas_startu_animacji() {
#if HARDW
        start = millis();
#else
        start = std::chrono::steady_clock::now();
#endif
        // stare_pozycje.assign(
        //     liczba_grup,
        //     -1);

        zgas_wszystkie();
    }
    //******************************************************
    bool klatka_animacji(double T,
                         int smuga,
                         int liczba_grup = 1,
                         double opoznienie = 0.3) {
        bool jakas_grupa_jest_na_listwie = false;
#if HARDW

        unsigned long teraz = millis();
        double t = (teraz - start) / 1000.0;
#else
        auto teraz = std::chrono::steady_clock::now();

        double t =
            std::chrono::duration<double>(
                teraz - start)
                .count();
#endif
        zgas_wszystkie();

        double a =
            2.0 * liczba_ledow_obiektu / (T * T);

        for (int grupa = 0;
             grupa < liczba_grup;
             ++grupa) {
            double start_grupy =
                grupa * opoznienie;

            if (t < start_grupy) {
                // S.printf("%s continue ", __PRETTY_FUNCTION__);
                continue;
            }

            double tg =
                t - start_grupy;

            double x =
                0.5 * a * tg * tg;

            int pozycja =
                static_cast<int>(x);

            if (pozycja < liczba_ledow_obiektu + smuga) {
                jakas_grupa_jest_na_listwie = true;

                zapal_led_nr(
                    pozycja,
                    bialy,
                    smuga);

                // stare_pozycje[grupa] = pozycja;
            }
        }

        if (!jakas_grupa_jest_na_listwie) {
            zgas_wszystkie();
            return false;
        }

        return true;
    }
    //******************************************************
    void wypisz_tresc() override {
        string tresc;

        for (int p = 0;
             p < liczba_ledow_obiektu;
             ++p) {
            if (Led[p].on)
                tresc.append("o");
            else
                tresc.append("_");
        }

        S.print(format("\n\nAkceleracja: {} \n", tresc).c_str());
    }
};
//***************************************************************************************************
//***************************************************************************************************
//***************************************************************************************************
Trura_akceleracyjna rura_akceleracyjna(1, 0, 127);

Tlistwa_dzielnika dzielnik(2, 0, 110, "Dzielnik ");


Tlistwa_odbierajaca listwa_odbierajaca(3, 0, 40, "Listwa odbieraj ");

Tlistwa_nakladajaca_ladunki listwa_nakladajaca(4, 3, 40, "Listwa naklad ");
Tlistwa_nakladajaca_ladunki oswietlenie(4, 0, 2, "Oswietlenie ");

Tczasza czasza(5, 0, 50, "Czasza ");  // Ma być kanal 5 !!!

Tbutla butla(6, 0, 23, "Butla");

Trurka_wodoru rurka_wodoru(6, 24, 59, "Rurka ");
Tzrodlo_jonow zrodlo_jonow(6, 60, 71, "Zrodlo ");
// Trurka_wodoru rurka_wodoru(6, 24+6, 27+6, "Rurka ");
// Tzrodlo_jonow zrodlo_jonow(6, 24, 27, "Zrodlo ");

int Telektroda::ile_elektrod;
vector<Telektroda> elektroda(11);  // ile ma byc elektrod (0-11)
// w konstuktroze domniemanym kanal poczatkowy = 7;
int ostatnia_elektroda;

Tpas pas(8);                          //  <-- arg to nr kanalu


//------------
vector<Telement_animowany*> anim;

//***************************************************************************************************
//***************************************************************************************************
//*********************************************************************************************
void ogolna_petla_animujaca(double ile_sekund, const vector<Telement_animowany*>& vec) {
#if HARDW
    unsigned long start = millis();
#else
    auto start = chrono::steady_clock::now();
#endif

    while (true) {
#if HARDW
        unsigned long teraz = millis();
        auto t = (teraz - start) / 1000;
#else
        auto teraz = chrono::steady_clock::now();
        double t = chrono::duration<double>(teraz - start).count();
#endif

        // S.println("Czas tej animacji = " << t );

        if (t >= ile_sekund)  // sekund
        {
            // S.printf("Minal czas animacj %d sekubd", t);

            break;
        }  // for(int i = 0 ; i < ile_razy ; ++i)

        for (auto* element : vec)
            element->kolejna_klatka();

        for (auto* element : vec)
            element->update_diody_bez_wyslania();
        // S.println(endl;
        H.wyslij();

        czekaj_milisekund(15);  // bylo: 50;
    }
}

//***************************************************************************************

//****************************************************************
struct Tplay {
    string tytul;
    int nr_nagrania;
    bool flag_trwa_nagranie = 0;

    void rozpocznij_nagranie_nr(int nr, string komentarz) {

        S.print(format("PLAY: {}, {}", nr, komentarz).c_str());

        nr_nagrania = nr;
        tytul = komentarz;
        if (flag_angielski) nr_nagrania += 100;
        play_track_number(nr_nagrania);
    }
    //**********************************************************************************
    void play_track_number([[maybe_unused]] int number) {
#if HARDW
        S.print("Odtwarzamy plik ");
        S.println(number);

        // Ustawienie 8-bitowego kodu numeru nagrania
        for (int i = 0; i < 8; i++) {
            digitalWrite(PIN_AUDIO_BASE + i, bitRead(number, i) ? LOW : HIGH);
        }

        // // Diagnostyka: odczyt stanów wszystkich 8 GPIO
        // S.print("GPIO 2..9: ");
        // for (int i = 0; i < 8; i++) {
        //   S.print(digitalRead(PIN_AUDIO_BASE + i));
        //   if (i < 7)
        //     S.print(" ");
        // }
        // S.println();

        digitalWrite(PIN_STATUS_LED, HIGH);  //  zapalamy led

        flag_trwa_nagranie = true;
        // Czas potrzebny na odczytanie numeru nagrania przez moduł
        czekaj_milisekund(120);

        // Powrót wszystkich linii do stanu spoczynkowego
        for (int i = 0; i < 8; i++) {
            digitalWrite(PIN_AUDIO_BASE + i, HIGH);
        }

        // Diagnostyka: sprawdzamy, czy wszystkie linie wróciły do HIGH
        // S.print("GPIO 2..9 po resecie: ");
        // for (int i = 0; i < 8; i++) {
        //   S.print(digitalRead(PIN_AUDIO_BASE + i));
        //   if (i < 7)
        //     S.print(" ");
        // }
        // S.println();

#endif
    }
    //****************************************************************
    void zaczekaj_na_zakonczenie_nagrania() {
#if HARDW

        int licznik = 0;
        // Najpierw czekamy, aż nagranie rzeczywiście się rozpocznie.
        // BUSY: HIGH -> LOW

        if (flag_trwa_nagranie != false) {
            while (digitalRead(PIN_AUDIO_BUSY) == HIGH) {
                czekaj_milisekund(20);  // 20
                // S.println("czekam na gotowosc A...");
                if (++licznik > 10) break;
            }
        }

        S.print("Rozpoczelo sie nagranie nr ");
        S.println(nr_nagrania);

        // Teraz czekamy na zakończenie nagrania.
        // BUSY: LOW -> HIGH
        while (digitalRead(PIN_AUDIO_BUSY) == LOW) {
            // S.println("trwa odtwarzanie czekam B...");
            czekaj_milisekund(100);
        }

        S.print(format("Zakonczylo sie nagranie nr {} tytul = {} \n", nr_nagrania, tytul).c_str());

        digitalWrite(PIN_STATUS_LED, LOW);  //  gasimy  led
        flag_trwa_nagranie = false;

        czekaj_milisekund(400);  // konieczna pauza do regeneracji modułu dźwiękowego  // bylo  500

#else

        S.print(format("Zakonczylo sie nagranie nr {} tytul = {} \n", nr_nagrania, tytul));

#endif
    }
};
//************************************************************************************************************************************
Tplay play;

void pelna_wstepna_animacja(int ile);
void wszystko_gasnie();
//////////////////////////////////////////////////////
void wykonaj_test();
#if HARDW == false
//*********************************************************************************************************************
int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // dla polskich liter
    SetConsoleCP(CP_UTF8);
#endif


    srand(time(nullptr));
    ostatnia_elektroda = elektroda.size() - 1;

    //------------------------------

    wykonaj_test();
    oswietlenie.zapal_wszystkie(bialy);
    wykonaj_czesc_pierwsza();

    // Gaśnie wszystko
    wszystko_gasnie();
    // POWTORNE WCISNIECIE KLAWISZA
    wykonaj_czesc_druga();

    play.rozpocznij_nagranie_nr(33, "MUZYKA FINALOWA....");
    oswietlenie.zgas_wszystkie();
    H.wyslij();

    return 0;
}

#endif  // HARDW == 0
//*****************************************************************
void pelna_wstepna_animacja(int ile_sekund) {

    // Na finałowej animacji działają prawie wszystkie obiekty, choć niektóre już się nie animują.
    // Zatem
    // a) listwa_nakładająca ładunki cała ciągle świeci na czerwono
    // b) pas - animuje przenoszenie tego z prawa na lewo
    // c) listwa odbierająca "iskrzy"
    // d) czasza świeci ciągle na czerwono
    // e) butla - swiecie słabo na niebiesko
    // f) rurka_wodoru - animuje przepływ gazu
    // g) źródło jonów świeci ciągle na pomarańczowo
    // g2) dzielnik napięcia ma na stało wyświetloną tęczę
    // h) elektrody świecą ciągle ustalonym wcześniej dla siebie kolorami świateł.
    // i) rura akceleracyjna - najpierw kilka razy powoli animuje rozpędzanie,
    //    a potem już przełącza się na tryb szybki i tak robi już do końca.

    listwa_nakladajaca.zapal_wszystkie(czerwony, 255);
    H.wyslij();
    // to świecenie ma trwać
    //==============================

    pas.pozycja_ladunkow = 0;
    pas.start_animacji_ladunkow(3, &listwa_odbierajaca);  // gestosc
    pas.ustaw_animacje_ladunkow(true,                     // animuj_listwe
                                false);                   // wstepne_miganie);
    pas.intens = 50;

    butla.zapal_wszystkie(zielony, 50);  // zmniejszamy jasnosc
    butla.update_diody_bez_wyslania();
    zrodlo_jonow.zapal_wszystkie(pomaranczowy);
    dzielnik.zapal_wszystkie_teczowo();
    for (int n = 0; n < elektroda.size() ; ++n) {
        int poz_koloru = n * dzielnik.liczba_ledow_obiektu / ostatnia_elektroda;
        Tkolor kolor = dzielnik.daj_kolor(poz_koloru);
        elektroda[n].zapal_wszystkie(kolor);
        // S.printf("eletroda nr %d kolor %d %d %d\n", n, kolor.r, kolor.g, kolor.b);
        elektroda[n].update_diody_bez_wyslania();
    }

    H.wyslij();

    vector<Telement_animowany*> wszystko = {
        &listwa_nakladajaca,
        &pas,
        &butla,
        &rurka_wodoru,
        // &zrodlo_jonow,  // - nie, bo za każdym razem wysyła (czyli spowalnia)
        // &dzielnik
        &rura_akceleracyjna
    };

    // S.printf("Przed petla animujaca %d \n", __LINE__);

    rura_akceleracyjna.start_animacji_szybkiej();
    ogolna_petla_animujaca(ile_sekund, wszystko);  // ===================== szybka =====================
}
//***************************************************************
void wszystko_gasnie() {
    listwa_nakladajaca.zgas_wszystkie();
    listwa_odbierajaca.zgas_wszystkie();
    pas.zerowanie_ledow();
    czasza.zgas_wszystkie();
    butla.zgas_wszystkie();
    rurka_wodoru.zgas_wszystkie();
    zrodlo_jonow.zgas_wszystkie();
    rura_akceleracyjna.zgas_wszystkie();

    for (auto& e : elektroda) e.zgas_wszystkie();
    dzielnik.zgas_wszystkie();

#if HARDW
    H.clearAll();
#endif
}
//*******************************************************************************

#if HARDW

void setup() {

    S.begin(115200);  // Inicjalizacja portu szeregowego na 9600 baudów
    // Inicjalizacja generatora liczb losowych
    randomSeed(analogRead(A0));


    // Numer ostatniej elektrody
    ostatnia_elektroda = elektroda.size() - 1;

    //======= wg Mariusza ===================

    pinMode(PIN_BUTTON_POLSKI, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ANGIELSKI, INPUT_PULLUP);
    // przycisk puszczony  → HIGH
    // przycisk wciśnięty  → LOW


    pinMode(PIN_STATUS_LED, OUTPUT);

    pinMode(PIN_AUDIO_BUSY, INPUT);

    for (int i = 0; i < 8; i++) {
        pinMode(PIN_AUDIO_BASE + i, OUTPUT);
        digitalWrite(PIN_AUDIO_BASE + i, HIGH);
    }

    // Inicjalizacja wszystkich taśm LED
    H.inicjalizuj();
    H.wyczysc();

    FastLED.setBrightness(255);
    H.clearAll();

    H.wyslij();

    //--------------------

    // Początkowo wszystko ma być wygaszone
    wszystko_gasnie();
    H.wyslij();

    S.printf("ZAkonczone setup - linia nr %d \n", __LINE__);
}
#endif
//*******************************************************************************
//*******************************************************************************
#if HARDW
bool przycisk_wcisniety(int pin) {

    if (digitalRead(pin) != LOW)
        return false;

    czekaj_milisekund(50);

    return digitalRead(pin) == LOW;
}
//************************************************************************************
void loop() {

    constexpr unsigned long CZAS_OCZEKIWANIA_NA_DRUGIE_WCISNIECIE = 15000;

    // ============================================================
    // PIERWSZE NACIŚNIĘCIE — wybór języka
    // ============================================================

    czekaj_milisekund(50);

    if (przycisk_wcisniety(PIN_BUTTON_POLSKI)) {

        flag_angielski = false;

        // czekamy na zwolnienie przycisku
        while (digitalRead(PIN_BUTTON_POLSKI) == LOW)
            czekaj_milisekund(10);

    } else if (przycisk_wcisniety(PIN_BUTTON_ANGIELSKI)) {

        flag_angielski = true;

        // czekamy na zwolnienie przycisku
        while (digitalRead(PIN_BUTTON_ANGIELSKI) == LOW)
            czekaj_milisekund(10);

    } else {
        return;
    }

    S.println("JEST pierwsze Wcisniecie przycisku !!!");

    // ============================================================
    // PIERWSZA CZĘŚĆ POKAZU
    // ============================================================

    oswietlenie.zapal_wszystkie(bialy);
    // wykonaj_test();
    wykonaj_czesc_pierwsza();


    // ============================================================
    // CZEKAMY NA DRUGIE NACIŚNIĘCIE
    // ============================================================

    S.println("czekamy na ponowne naciśnięcie przycisku...");

    unsigned long poczatek_oczekiwania = millis();
    bool drugie_wcisniecie = false;

    while (millis() - poczatek_oczekiwania < CZAS_OCZEKIWANIA_NA_DRUGIE_WCISNIECIE) {

        if (przycisk_wcisniety(PIN_BUTTON_POLSKI)) {

            flag_angielski = false;
            drugie_wcisniecie = true;

            while (digitalRead(PIN_BUTTON_POLSKI) == LOW)
                czekaj_milisekund(10);

            break;
        }

        if (przycisk_wcisniety(PIN_BUTTON_ANGIELSKI)) {

            flag_angielski = true;
            drugie_wcisniecie = true;

            while (digitalRead(PIN_BUTTON_ANGIELSKI) == LOW)
                czekaj_milisekund(10);

            break;
        }

        czekaj_milisekund(10);
    }

    // ============================================================
    // DRUGA CZĘŚĆ ALBO KONIEC
    // ============================================================

    if (drugie_wcisniecie) {

        wykonaj_czesc_druga();

        wszystko_gasnie();
        oswietlenie.zgas_wszystkie();
        H.wyslij();

        return;
    }

    // Nie było drugiego naciśnięcia

    oswietlenie.zgas_wszystkie();
    wszystko_gasnie();

    H.wyslij();

    play.rozpocznij_nagranie_nr(99, "Muzyka...");
    play.zaczekaj_na_zakonczenie_nagrania();
}
#endif
//**************************************************************************
//*********************************************************************************
void wykonaj_czesc_pierwsza() {


    S.print(format("Prawdziwa Czesc pierwsza - linia nr {}  \n", __LINE__).c_str());

    play.rozpocznij_nagranie_nr(1, "Przed panstwem...");
    play.zaczekaj_na_zakonczenie_nagrania();

    play.rozpocznij_nagranie_nr(2, "Aby to bylo mozliwe...");
    // pelna_wstepna_animacja(25);
    pelna_wstepna_animacja(5);
    play.zaczekaj_na_zakonczenie_nagrania();

    wszystko_gasnie();
    H.wyslij();

    // Pytanie
    play.rozpocznij_nagranie_nr(3, "Nacisnij powtornie...");
    play.zaczekaj_na_zakonczenie_nagrania();
}
//************************************************************************
void wykonaj_czesc_druga() {

    wszystko_gasnie();   // nawet oswietlenie?
    S.print(format("Prawdziwa Czesc druga - linia nr {}  \n", __LINE__).c_str());

    pas.zerowanie_ledow();
    H.wyslij();

    pas.reset();

#if 1

    play.rozpocznij_nagranie_nr(4, "Na czym polega...");
    play.zaczekaj_na_zakonczenie_nagrania();  // ################################################

    anim.clear();
    anim.push_back(&pas);

    S.println("Akcja III. Pas swieci na bialo");

    pas.zaswiec_na_bialo();
    H.wyslij();

    pas.tryb_animacji = Tpas::Tryb_animacji::brak;
    ogolna_petla_animujaca(2, anim);  //  sekundy bialego pasaa

    pas.zerowanie_ledow();
    H.wyslij();
    S.println("pas.zerowanie_ledow() ");

    // Strzalki ----------------------

    play.rozpocznij_nagranie_nr(5, "Pas poruszany jest ...");  //##################################

    //S.println("Akcja III. Pas ruch strzałek " );
    S.println("Akcja IV. Pas ruch STRZALEK\n ");
    pas.intens = 255;
    pas.start_animacji_strzalek();
    ogolna_petla_animujaca(30, anim);

    play.zaczekaj_na_zakonczenie_nagrania();  // ################################################


    S.println("Akcja V. Pas gasnie ");
    pas.zerowanie_ledow();
    pas.update_diody_bez_wyslania();
    H.wyslij();

    //--------------------------------------
    play.rozpocznij_nagranie_nr(6, "Tak dzialaly pierwsze akceleratory");  // 7 od tej pory przenumerować nagrania
    play.zaczekaj_na_zakonczenie_nagrania();

    play.rozpocznij_nagranie_nr(7, "Ta listwa  przekazuje...");

    S.println("A VII. Miga LIstwa nakladajaca ladunki");
    listwa_nakladajaca.zamigaj_i_update(bialy, 200, 5, true);
    H.wyslij();

    play.zaczekaj_na_zakonczenie_nagrania();



    S.println("A VII. LIstwa nakladajaca ladunki swieci ciagle");
    listwa_nakladajaca.zapal_wszystkie(czerwony, 255);
    H.wyslij();
    // to świecenie ma trwać

    play.zaczekaj_na_zakonczenie_nagrania();

    play.rozpocznij_nagranie_nr(8, "Poniewaz caly czas...");

    S.println("A VIII. Pas przesuwa ladunki");
    pas.pozycja_ladunkow = 0;
    pas.start_animacji_ladunkow(3, &listwa_odbierajaca);  // gestosc
    pas.ustaw_animacje_ladunkow(false,                    // animuj_listwe
                                true);                    // wstepne_miganie);
    pas.intens = 255;

    anim.clear();
    anim.push_back(&listwa_nakladajaca);
    anim.push_back(&pas);
    anim.push_back(&listwa_odbierajaca);

    ogolna_petla_animujaca(8, anim);  // 8 sekund
    play.zaczekaj_na_zakonczenie_nagrania();


    play.rozpocznij_nagranie_nr(9, "Co dalej z tymi ladunkami...");  //
    czekaj_milisekund(3000);                                         // zeby czasza migała 3 sekundy później
    czasza.zamigaj_i_update(bialy, 100, 5, false);
    play.zaczekaj_na_zakonczenie_nagrania();

    S.print("A X. Lewa listwa miga \n");
    listwa_odbierajaca.zamigaj_i_update(bialy, 100, 5, false);

    play.rozpocznij_nagranie_nr(10, "Tutaj widzimy druga listwe...");  // 11

    S.println("A XI. pas nadal dostacza ladunki a Lewa listwa iskrzy na czerwono ");
    pas.ustaw_animacje_ladunkow(true,   // animuj_listwe
                                true);  // wstepne_miganie);


    ogolna_petla_animujaca(10, anim);

    S.println("A. XII. Iskrzenie trwa nadal");
    pas.ustaw_animacje_ladunkow(true,    // animuj_listwe
                                false);  // bez wstepnego migania);
    ogolna_petla_animujaca(16, anim);

    S.println("A XIII. pas nadal dostacza ladunki a Lewa listwa iskrzy na czerwono, czasza sie rozjarza");
    czasza.wyzeruj_rozjarzanie();
    anim.push_back(&czasza);
    ogolna_petla_animujaca(42, anim);  // potrzebne co najmniej 12 sekund

    //================================================
    S.println("A XIV.  Wszystkie LEDy gasna");
    wszystko_gasnie();  // także hardware


    czasza.wyzeruj_rozjarzanie();
    play.zaczekaj_na_zakonczenie_nagrania();

    //=======WODOR ========================================

    play.rozpocznij_nagranie_nr(11, "\nBedziemy przyspieszac");
    play.zaczekaj_na_zakonczenie_nagrania();

    S.println("A XV.Butla miga ");
    butla.zamigaj_i_update(zielony, 100, 3, true);


    play.rozpocznij_nagranie_nr(12, "Gaz z butli przepływa...");  //

    S.println("A XVI. Rurka animuje przeplyw wodoru");
    vector<Telement_animowany*> anim_wodoru;

    anim_wodoru.push_back(&butla);
    anim_wodoru.push_back(&rurka_wodoru);

    butla.zapal_wszystkie(zielony, 50);  // zmniejszamy jasnosc
    butla.update_diody_bez_wyslania();

    rurka_wodoru.start_animacji();
    ogolna_petla_animujaca(10, anim_wodoru);

    play.zaczekaj_na_zakonczenie_nagrania();
    play.rozpocznij_nagranie_nr(13, "    Zwykły atom wodoru jest elektrycznie ... Miganie zrodla");  // 14
    S.println("A XVII.Zrodlo jonow miga ");

    // zrodlo_jonow.zamigaj_i_update(pomaranczowy, 200, 2, true);

    //zapal_wszystkie(pomaranczowy, 255);
    zrodlo_jonow.reset();
    anim_wodoru.push_back(&zrodlo_jonow);
    ogolna_petla_animujaca(28, anim_wodoru);

    play.zaczekaj_na_zakonczenie_nagrania();

    wszystko_gasnie();
    S.println("A XVIII.Rura akceleracyjna miga ");
    rura_akceleracyjna.zamigaj_i_update(bialy, 100, 6, false);
    // rura_akceleracyjna.tecza();

    czekaj_milisekund(500);
    wszystko_gasnie();


    play.rozpocznij_nagranie_nr(14, "Przyspieszanie jonow odbywa sie w rurze...");
    S.println("A XIX.Czasza rozjarza się");
    czasza.wyzeruj_rozjarzanie();
    czasza.ustaw_przyrost_intensywnosci(50);  // int przyrost);
    anim.clear();
    anim.push_back(&czasza);
    //czasza.zapal_od_razu();
    ogolna_petla_animujaca(10, anim);
    play.zaczekaj_na_zakonczenie_nagrania();

#endif

    S.println("A XIX.dodatkowo pierwsza elektroda zapala sie na czerwono");
    play.rozpocznij_nagranie_nr(15, "Gdy nasz dodatnio naladowany proton ...");
    play.zaczekaj_na_zakonczenie_nagrania();

    elektroda[0].zapal_wszystkie(czerwony);
    elektroda[0].update_diody_bez_wyslania();
    H.wyslij();


    play.rozpocznij_nagranie_nr(16, "A na koncu ziemia ...");
    play.zaczekaj_na_zakonczenie_nagrania();
    S.println("A XX.dodatkowo ostatnia elektroda zapala sie na niebiesko");
    elektroda[ostatnia_elektroda - 1].zapal_wszystkie(niebieski);
    elektroda[ostatnia_elektroda - 1].update_diody_bez_wyslania();

    elektroda[ostatnia_elektroda].zapal_wszystkie(zielony);  // Jeśli rzeczywiscie będzie
    elektroda[ostatnia_elektroda].update_diody_bez_wyslania();
    H.wyslij();



    //  miganie jednego protonu na początku

    for (int i = 0; i < 6; ++i) {
        rura_akceleracyjna.zapal_led_nr(1, bialy);
        czekaj_milisekund(200);
        rura_akceleracyjna.zapal_led_nr(1, czarny);
        czekaj_milisekund(200);
    }


    rura_akceleracyjna.start_animacji_pojedynczej();

    // ruch przyspieszony pojedynczego protonu
    auto
        sama_rura = anim;
    sama_rura.clear();
    sama_rura.push_back(&rura_akceleracyjna);
    ogolna_petla_animujaca(6, sama_rura);



    play.rozpocznij_nagranie_nr(17, "Technicy...stosuje się...");
    S.println(" A XXI.  Elektrody gasna");
    elektroda[0].zgas_wszystkie();
    elektroda[0].update_diody_bez_wyslania();

    // na wszelki wypadek
    elektroda[ostatnia_elektroda - 1].zgas_wszystkie();
    elektroda[ostatnia_elektroda - 1].update_diody_bez_wyslania();

    // prawdziwa ostatnia
    elektroda[ostatnia_elektroda].zgas_wszystkie();
    elektroda[ostatnia_elektroda].update_diody_bez_wyslania();
    H.wyslij();
    play.zaczekaj_na_zakonczenie_nagrania();


    S.println("A XXII.Dzielnik napiecia miga ");
    dzielnik.zamigaj_i_update(bialy, 100, 2, false);


    play.rozpocznij_nagranie_nr(18, "Lewy koniec dzielnika...");

    S.println("Wysokie napiecie jest rownomiernie rozkladane");


    dzielnik.start_animacji();
    anim.push_back(&dzielnik);  // jest czasza + dzielnik
    ogolna_petla_animujaca(20, anim);

    play.zaczekaj_na_zakonczenie_nagrania();


    // skok:

    play.rozpocznij_nagranie_nr(19, "Napiecia z kolejnych punktow dzielnika...");
    S.println("A XXIII. Kolejne elektrody otrzymuja kolor z dzielnika ");
    // kolejne fazy napiecia na elektrody

    for (int n = 0; n <= ostatnia_elektroda; ++n) {

        int poz_koloru = n * dzielnik.liczba_ledow_obiektu / ostatnia_elektroda;
        Tkolor kolor = dzielnik.daj_kolor(poz_koloru);
        elektroda[n].info();
        elektroda[n].zgas_wszystkie();
        for (int le = 0; le < elektroda[n].liczba_ledow_obiektu; ++le) {
            // elektroda[n].zapal_wszystkie(kolor);

            elektroda[n].zapal_led_nr(le, kolor);
            H.wyslij();
        }
        elektroda[n].wypisz_tresc();
        // S.print( format("elektroda nr {} kolor {} {} {} \n", n, kolor.r, kolor.g, kolor.b).c_str() );
        // elektroda[n].update_diody_bez_wyslania();

        czekaj_milisekund(1000);
    }

    play.zaczekaj_na_zakonczenie_nagrania();

    play.rozpocznij_nagranie_nr(20, "W rzeczywistym aklceleratorze ekektrod....");
    play.zaczekaj_na_zakonczenie_nagrania();

    // #endif

    // #endif


#if 1


    listwa_nakladajaca.zapal_wszystkie(czerwony, 155);
    H.wyslij();
    // to świecenie ma trwać

    pas.pozycja_ladunkow = 0;
    pas.start_animacji_ladunkow(3, &listwa_odbierajaca);  // gestosc


    play.rozpocznij_nagranie_nr(21, "Teraz wszystko jest już gotowe.....");

    pas.ustaw_animacje_ladunkow(true,    // animuj_listwe odbierajaca
                                false);  // bez wstepnego migania);
    pas.intens = 40;

    butla.zapal_wszystkie(zielony, 50);  // zmniejszamy jasnosc
    butla.update_diody_bez_wyslania();
    zrodlo_jonow.zapal_wszystkie(pomaranczowy);
    dzielnik.zapal_wszystkie_teczowo();

    vector<Telement_animowany*> wszystko = {
        &listwa_nakladajaca,
        &pas,
        &butla,
        &rurka_wodoru,
        // &zrodlo_jonow,  // - nie, bo za każdym razem wysyła (czyli spowalnia)
        // &dzielnik

    };

    // S.printf("Przed petla animujaca %d \n", __LINE__);

    ogolna_petla_animujaca(10, wszystko);  // ROZBIEG =========================


    // S.printf("PO petli animujacej %d przed akceleraca\n", __LINE__);

    rura_akceleracyjna.start_animacji_pojedynczej();
    // ruch przyspieszony pojedynczego protonu -----------------------------------------------------

    wszystko.push_back(&rura_akceleracyjna);
    wszystko.push_back(&czasza);
    ogolna_petla_animujaca(6, wszystko);  // ====JEDEN ===========================

    S.printf("Po pojedynczej petli animujacej %d \n", __LINE__);

    //-----------------
    rura_akceleracyjna.start_animacji_wolnej();

    ogolna_petla_animujaca(10, wszystko);  //============ wolna ===============================

    S.printf("Po wolnej petli animujacej %d \n", __LINE__);

    rura_akceleracyjna.start_animacji_szybkiej();
    ogolna_petla_animujaca(45 + 30, wszystko);  // ===================== szybka =====================

    play.zaczekaj_na_zakonczenie_nagrania();

    play.rozpocznij_nagranie_nr(99, "MUZYKA FINALOWA....");
    play.zaczekaj_na_zakonczenie_nagrania();


#endif
}
//****************************************************************************************************************
void wykonaj_test() {

    // goto skok;

    // zamigaj_i_update(Tkolor kolor,
    //               int milisekund,
    //               double ile_sekund,
    //               bool flag_zostawic_swiecace)

    // rura_akceleracyjna.zamigaj_i_update(bialy, 200, 10, true);    // (1, 0, 127);
    // dzielnik.zamigaj_i_update(niebieski, 200, 10, true);          // (2, 0, 110, "Dzielnik ");

    // return;

    listwa_odbierajaca.zamigaj_i_update(zielony, 200, 10, true);  // (3, 0, 40, "Listwa odbieraj ");

    listwa_nakladajaca.zamigaj_i_update(zolty, 200, 10, true);  // (4, 3, 40, "Listwa naklad ");
    // oswietlenie.zamigaj_i_update(zolty, 200, 10, true);      //(4, 0, 2, "Oswietlenie ");z
    czasza.zamigaj_i_update(czerwony, 200, 10, true);
skok:
    butla.zamigaj_i_update(zielony, 200, 5, true);              // (6, 0, 3, "Butla");
    rurka_wodoru.zamigaj_i_update(bialy, 200, 5, true);         //(6, 4, 10, "Rurka ");
    zrodlo_jonow.zamigaj_i_update(pomaranczowy, 200, 5, true);  // (6, 11, 14, "Zrodlo ");
    return;

    //int Telektroda::ile_elektrod;
    // vector<Telektroda> elektroda(12);  // ile ma byc elektrod (0-11)
    // w konstuktroze domniemanym kanal poczatkowy = 7;
    // for (int n = 0; n < elektroda.size(); ++n) {
    //   int r = 255 - n * 18;
    //   int b =  n * 18;
    //   Tkolor kol(r, 0, b);
    //   elektroda[n].zamigaj_i_update(kol, 200, 3, true);
    //   S.print(format("Elektroda {} R={} B =", n, r, b).c_str() );
    //   czekaj_milisekund(500);
    // }

    elektroda[0].zamigaj_i_update(czerwony, 200, 3, true);
    elektroda[1].zamigaj_i_update(niebieski, 200, 3, true);
    elektroda[2].zamigaj_i_update(zielony, 200, 3, true);
    elektroda[3].zamigaj_i_update(cyjan, 200, 3, true);

    elektroda[4].zamigaj_i_update(czerwony, 200, 3, true);
    elektroda[5].zamigaj_i_update(niebieski, 200, 3, true);
    elektroda[6].zamigaj_i_update(zielony, 200, 3, true);
    elektroda[7].zamigaj_i_update(cyjan, 200, 3, true);

    elektroda[8].zamigaj_i_update(czerwony, 200, 3, true);
    elektroda[9].zamigaj_i_update(niebieski, 200, 3, true);
    elektroda[10].zamigaj_i_update(zolty, 200, 3, true);

    czekaj_milisekund(500);



    play.rozpocznij_nagranie_nr(99, "MUZYKA FINALOWA....");
    play.zaczekaj_na_zakonczenie_nagrania();
    wszystko_gasnie();

    return;
    for(int i = 0 ; i < 10 ;++i)
    {
        pas.zaswiec_na_bialo();           // (8);  //  <-- arg to nr kanalu
        pas.update_diody_bez_wyslania();
        H.wyslij();
        czekaj_milisekund(500);

        pas.zerowanie_ledow();
        pas.update_diody_bez_wyslania();
        H.wyslij();
        czekaj_milisekund(500);
    }

    return;

    czasza.zamigaj_i_update(zielony, 200, 10, true);  // (5, 0, 50, "Czasza ");  // Ma być kanal 5 !!!

    //#####################################################################################
    // S.print("rura_akceleracyjna - zapal biale  \n");
    // rura_akceleracyjna.zapal_wszystkie(bialy);
    // rura_akceleracyjna.update_diody_bez_wyslania();
    // H.wyslij();

    // czekaj_milisekund(100);

    // S.print("rura_akceleracyjna - zapal czerwone  \n");


    // rura_akceleracyjna.zapal_wszystkie(czerwony);
    // rura_akceleracyjna.update_diody_bez_wyslania();
    // H.wyslij();

    // czekaj_milisekund(100);


    // rura_akceleracyjna.zapal_wszystkie(czarny);
    // rura_akceleracyjna.update_diody_bez_wyslania();
    // H.wyslij();
    // czekaj_milisekund(100);


    // //----------------------------------------------------------------------
    // S.print("listwa_nakladajaca  - zapal zielone   \n");
    // listwa_nakladajaca.zapal_wszystkie(zielony);
    // listwa_nakladajaca.update_diody_bez_wyslania();
    // H.wyslij();

    // S.println("Akcja II. Pas " );
    // pas.zaswiec_na_bialo();
    // pas.update_diody_bez_wyslania();
    // H.wyslij();

    // zrodlo_jonow.zapal_wszystkie(czerwony);
    // zrodlo_jonow.update_diody_bez_wyslania();
    // H.wyslij();

    // butla.zapal_wszystkie(bialy);
    // butla.update_diody_bez_wyslania();
    // H.wyslij();

    // rurka_wodoru.zapal_wszystkie(niebieski);
    // rurka_wodoru.update_diody_bez_wyslania();
    // H.wyslij();

    // rura_akceleracyjna.zapal_wszystkie(niebieski);
    // rura_akceleracyjna.update_diody_bez_wyslania();
    // H.wyslij();

    // for (unsigned int i = 0; i < elektroda.size(); ++i) {
    //   Tkolor k{ (short int)(i * 20),
    //             (short int)(250 - (i * 20)),
    //             (short int)0 };
    //   elektroda[i].zapal_wszystkie(k);
    //   elektroda[i].update_diody_bez_wyslania();
    //   H.wyslij();
    //   czekaj_milisekund(500);
    // }

    // czekaj_milisekund(2000);

    // wszystko_gasnie();
    // H.wyslij();
    // S.print("Koniec funkcji testowej ");
}


