#ifndef __WORDFRAME_H
#define __WORDFRAME_H

class WordFrame
{

public:

    typedef struct
    {
        uint8_t x;
        uint8_t y;
        uint8_t length;
    } Word;

    // define the matrix size
    static const uint8_t WIDTH  = 13;  // LEDs per row
    static const uint8_t HEIGHT = 11;  // LEDs per column

    // position and length of the words:
    //                        x   y  length
    const Word W0_ES      = { 0,  0, 2};
    const Word W0_IST     = { 3,  0, 3};
    const Word W0_DREI    = { 7,  0, 4};
    const Word W0_EIN     = { 9,  0, 3};
    const Word W0_EINE    = { 9,  0, 4};

    const Word W1_ZWANZIG = { 0,  1, 7};
    const Word W1_ZWEI    = { 7,  1, 4};
    const Word W1_EIN     = { 9,  1, 3};
    const Word W1_EINS    = { 9,  1, 4};

    const Word W2_SIEB    = { 0,  2, 4};
    const Word W2_SIEBEN  = { 0,  2, 6};
    const Word W2_NEUN    = { 5,  2, 4};
    const Word W2_NACH    = { 8,  2, 4};
    const Word W2_NACHT   = { 8,  2, 5};
    const Word W2_ACHT    = { 9,  2, 4};

    const Word W3_ZWOELF  = { 0,  3, 5};
    const Word W3_FUENF   = { 4,  3, 4};
    const Word W3_SECH    = { 8,  3, 4};
    const Word W3_SECHS   = { 8,  3, 5};

    const Word W4_VIER    = { 0,  4, 4};
    const Word W4_VIERTEL = { 0,  4, 7};
    const Word W4_ELF     = { 5,  4, 3};
    const Word W4_ZEHN    = { 9,  4, 4};

    const Word W5_MINUTE  = { 1,  5, 6};
    const Word W5_MINUTEN = { 1,  5, 7};
    const Word W5_VOR     = { 9,  5, 3};

    const Word W6_NACH    = { 0,  6, 4};
    const Word W6_NACHT   = { 0,  6, 5};
    const Word W6_ACHT    = { 1,  6, 4};
    const Word W6_HALB    = { 5,  6, 4};
    const Word W6_ELF     = {10,  6, 3};

    const Word W7_EIN     = { 0,  7, 3};
    const Word W7_EINS    = { 0,  7, 4};
    const Word W7_SECHS   = { 3,  7, 5};
    const Word W7_SIEBEN  = { 7,  7, 6};

    const Word W8_FUENF   = { 0,  8, 4};
    const Word W8_ZWEI    = { 4,  8, 4};
    const Word W8_DREI    = { 8,  8, 4};

    const Word W9_ZEHN    = { 1,  9, 4};
    const Word W9_NEUN    = { 4,  9, 4};
    const Word W9_NACHT   = { 7,  9, 5};
    const Word W9_ACHT    = { 8,  9, 4};

    const Word W10_VIER   = { 0, 10, 4};
    const Word W10_ZWOELF = { 4, 10, 5};
    const Word W10_UHR    = {10, 10, 3};

    WordFrame()
    {
        clear();
    }

    void clear()
    {
        for (uint8_t i = 0; i < HEIGHT; i++)
        {
            this->mask_[i] = 0x0000;
        }
    }

    bool isSet(uint8_t x, uint8_t y)
    {
        return ((this->mask_[y]) >> x) & 0x0001;
    }

    WordFrame& add(Word word)
    {
        uint16_t bits = (0x0001 << word.length) - 1;
        this->mask_[word.y] |= bits << word.x;
        return *this;  // to cascade function calls: myFrame.add(x).add(y).add(z);
    }

    WordFrame& fromTime(uint8_t hour, uint8_t minute)
    {
        clear();

        uint8_t minute2 = minute;
        if ((minute >= 21) && (minute <= 29))
        {
            minute2 = 30 - minute;
        }
        else if ((minute >= 31) && (minute <= 39))
        {
            minute2 = minute - 30;
        }
        else if ((minute >= 40) && (minute <= 59))
        {
            minute2 = 60 - minute;
        }

        uint8_t hour2 = hour;
        if (minute >= 21)
        {
            hour2 = (hour + 1) % 12;
        }

        add(W0_ES);
        add(W0_IST);

        switch (minute2)
        {
            case  1: add(W0_EINE);    break;
            case  2: add(W1_ZWEI);    break;
            case  3: add(W0_DREI);    break;
            case  4: add(W4_VIER);    break;
            case  5: add(W3_FUENF);   break;
            case  6: add(W3_SECHS);   break;
            case  7: add(W2_SIEBEN);  break;
            case  8: add(W2_ACHT);    break;
            case  9: add(W2_NEUN);    break;
            case 10: add(W4_ZEHN);    break;
            case 11: add(W4_ELF);     break;
            case 12: add(W3_ZWOELF);  break;
            case 13: add(W0_DREI);
                    add(W4_ZEHN);    break;
            case 14: add(W4_VIER);
                    add(W4_ZEHN);    break;
            case 15: add(W4_VIERTEL); break;
            case 16: add(W3_SECH);
                    add(W4_ZEHN);    break;
            case 17: add(W2_SIEB);
                    add(W4_ZEHN);    break;
            case 18: add(W2_ACHT);
                    add(W4_ZEHN);    break;
            case 19: add(W2_NEUN);
                    add(W4_ZEHN);    break;
            case 20: add(W1_ZWANZIG); break;
        }
        if (minute2 == 1)
        {
            add(W5_MINUTE);
        }
        else if (minute % 5 > 0)
        {
            add(W5_MINUTEN);
        }

        if (((minute >= 1) && (minute <= 20)) || ((minute >= 31) && (minute <= 39)))
        {
            add(W6_NACH);
        }
        if (((minute >= 21) && (minute <= 29)) || ((minute >= 40) && (minute <= 59)))
        {
            add(W5_VOR);
        }

        if ((minute >= 21) && (minute <= 39))
        {
            add(W6_HALB);
        }

        switch (hour2)
        {
            case  0: add(W10_ZWOELF);  break;
            case  1: add((minute == 0) ? W7_EIN : W7_EINS); break;
            case  2: add(W8_ZWEI);     break;
            case  3: add(W8_DREI);     break;
            case  4: add(W10_VIER);    break;
            case  5: add(W8_FUENF);    break;
            case  6: add(W7_SECHS);    break;
            case  7: add(W7_SIEBEN);   break;
            case  8: add(W9_ACHT);     break;
            case  9: add(W9_NEUN);     break;
            case 10: add(W9_ZEHN);     break;
            case 11: add(W6_ELF);      break;
        }

        if (minute == 0)
        {
            add(W10_UHR);
        }

        return *this;  // to cascade function calls: myFrame.fromTime(x, y).add(z);
    }


private:

    uint16_t mask_[HEIGHT];

};

#endif  // __WORDFRAME_H
