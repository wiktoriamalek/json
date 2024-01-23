#include <iostream>
#include <fstream>

using namespace std;


fstream plik;
void help ()
{
    cout << "------HELPDESK--------" << endl;
    cout << "Dane autora: " << endl;
    cout << "Wiktoria Malek, nr 193323, ACiR, grupa 2" << endl;
    cout << "Wcisnij 'o', aby wyswietlic operacje o/ nazwa_pliku." <<endl;
    cout << "Wcisnij 'c', aby wyswietlic operacje c. " << endl;
    cout << "----------------------" << endl;
};


void sprawdz_string()
{
   
    int i = 0;
    char c;
    char tab[32];
    
    while (i < 6) // max 6 znakow
    {
        plik >> c;
        tab[i] = c;
        i++;
        
        if (c == '"')
            break;
    }
    
};




void operacja_o ()
{
    plik.open("projekt.json", ios::in);
    
    if (plik.good()==true)
    {
        cout << "Plik zostal otwarty!" << endl;
    }
    else
    {
        cout << "Plik nie istnieje!" << endl;
    }
   
};


//funkcje do operacji C
 

bool validateJson(fstream& stream);
bool validateValue(fstream& stream);
bool validateValue(fstream& stream, bool); //dla calego pliku, poniewaz istnieja przypadki gdy plik zawiera tylko stringa, boola, null albo liczbe
bool validateKeyValue(fstream& stream);
bool validateArray(fstream& stream);
bool validateBoolOrNull(fstream& stream);
bool validateString(fstream& stream);
bool validateNumber(fstream& stream);

char peek(fstream& stream) {
    stream >> ws;
    return stream.peek();
}
const char _BOOL_FALSE[] = {'f', 'a', 'l', 's', 'e'};
const char _BOOL_TRUE[] = {'t', 'r', 'u', 'e'};
const char _NULL_NULL[] = {'n', 'u', 'l', 'l'};

enum Type {
    BOOL_TRUE,
    BOOL_FALSE,
    NULL_NULL
};




bool validateNumber(fstream& stream) {
    char current; bool minus = false, firstDigit = true, decimal = false; 


    stream >> skipws >> current;
    

    
    if ((current < '0' || current > '9') && current != '-')
        return false;
    if (current != '-') {
        firstDigit = false;
    }
    if (current == '-') {
        if (stream.peek() == EOF || stream.peek()< '0' || stream.peek()> '9') {
            return false;
        }
    }
    if (current == '0' && stream.peek()!= '.' && (stream.peek()>= '0' && stream.peek()<= '9'))
        return false;

    if (!firstDigit &&stream.peek()!= '.' && (stream.peek() < '0' || stream.peek() > '9'))
        return true;

    while (true) {
        
        if (stream.peek() == EOF)
            break;
        
        stream >> noskipws >>  current;
         


        if (firstDigit && (current < '0' || current > '9'))
            return false;

        if (firstDigit && current == '0' && (stream.peek()  != EOF && stream.peek() != '.' && stream.peek()  >= '0' && stream.peek()  <= '9'))
            return false;


        if (current == '.') {  
            if (decimal)
                return false;
            decimal = true;
            if (stream.peek()  == EOF || stream.peek() < '0' ||  stream.peek() > '9')
                return false;
        }

        if (firstDigit)
            firstDigit = false;

        if (stream.peek() != '.' && (stream.peek() < '0' || stream.peek() > '9')) {
            break;
        }

    }

    return true;
}
bool validateString(fstream& stream) {
    char current; 

    stream >> skipws >> current;
    
    
    
    if (current != '"')
        return false;

    while (true) {
        if (peek(stream) == EOF)
            return false;

        stream >> noskipws >> current; 
         

        if (current == '\\') {
            if(peek(stream) != '\\' && peek(stream) != '"' && peek(stream) != 't')
                return false;
            stream.ignore(1);
            
        }

        if (current == '\"')
            break;

    }

    return true;
}


const char* GET_TYPE(Type type) {
    if (type == BOOL_TRUE)
        return _BOOL_TRUE;
    if (type == BOOL_FALSE)
        return _BOOL_FALSE;
    return _NULL_NULL;
}
int GET_LENGTH(Type type) {
    if (type == BOOL_TRUE)
        return 4;
    if (type == BOOL_FALSE)
        return 5;
    return 4;
}

bool validateBoolOrNull(fstream& stream) {
    char current;
    
    stream >> skipws >> current;
    
    

    Type type;

    if (current == 'f')
        type = BOOL_FALSE;
    else if (current == 't')
        type = BOOL_TRUE;
    else if (current == 'n')
        type = NULL_NULL;
    else return false;

    for (int i = 1; true; i++) {
        if (peek(stream) == EOF)
            return false;

         
        stream >> noskipws >> current; 
        

        if (current != GET_TYPE(type)[i])
            return false;
        if (i == GET_LENGTH(type) -1 ) 
            break;
    }


    return true;
};


bool validateValue(fstream& stream) {
    char next = peek(stream); 

    switch (next) { // wybiera jaki typ powinien byc sprawdzony na podstawie pierwszego znaku
        case '"':
            //string
            if (!validateString(stream))
                return false;
            break;
        case 'f':
            //bool albo null (false)
            if (!validateBoolOrNull(stream))
                return false;
            break;
        case 'n':
            //bool albo null (null)
            if (!validateBoolOrNull(stream))
                return false;
            break;
        case 't':
            //bool albo null (true)
            if (!validateBoolOrNull(stream))
                return false;
            break;
        case '[':
            //array
            if (!validateArray(stream))
                return false;
            break;
        case '{':
            //json
            if (!validateJson(stream))
                return false;
            break;
        default:
            if ((next >= '0' && next <= '9') || next == '-') { // liczba 
                if (!validateNumber(stream))
                    return false;
            }
            else 
                return false; //zaden znany typ nie rozpoczyna sie nastepnym znakiem
            break;
    }

    return true;    
}
bool validateValue(fstream& stream, bool isTop) {
    return validateValue(stream) && peek(stream) == EOF;
}
bool validateKeyValuePair(fstream& stream) {
    // sprawdź klucz (musi być stringiem)
    if (!validateString(stream))
        return false;

    char current; 

    stream >> skipws >> current;
    

    if (current != ':')
        return false;

    if (!validateValue(stream)) {
        return false;
    }
    return true;
}

bool validateJson(fstream& stream) {
    char current;

    stream >> skipws >> current;  


    if (current != '{')
        return false;
    if (peek(stream)== '}') {
        stream >> ws;
        stream.ignore(1);
        return true;
    }
    

    
    while (true)
    {
        if (peek(stream) == EOF)
            return false;

        if (!validateKeyValuePair(stream))
            return false;

        if (peek(stream) == EOF)
            return false;

        stream >> skipws >> current; 
        
        

        if (current == '}')
            break;
        if (current == ',')
            continue;
        return false;
    }
    

    return true;
};
bool validateArray(fstream& stream) {
    char current;

    stream >> skipws >> current;  


    if (current != '[')
        return false;
    if (peek(stream) == ']') {
        stream >> ws;
        stream.ignore(1);
        return true;
    }
    

    
    while (true)
    {
        if (peek(stream) == EOF)
            return false;

        if (!validateValue(stream))
            return false;

        if (peek(stream) == EOF)
            return false;

        stream >> skipws >> current;
        


        if (current == ']')
            break;
        if (current == ',')
            continue;
        return false;
    }
    

    return true;
};



void operacja_c ()
{
   
    if (validateValue(plik, true)) {
            cout << "Jest ok\n";
    }
    else {
        cout << "nie jest ok\n";
        cout << "blad w " << plik.tellg() << endl;
    }

    };
    
    int main()
    {
        char znak;
        char operacja;
        cout << "Wcisnij h, aby wyswietlic helpa" << endl;
        
        while (true)
        {
            // system(clrscr);  -> moze czyscic ekran?
            cin >> znak;
            
            if (znak=='h')
            {
                help();
            }
            else if (znak=='o')
            {
                operacja_o();
            }
            else if (znak=='c')
            {
                operacja_c();
            }
            else if (znak == 'q')
            {
                break;
            }
            
            //to sprawdzic
          //  exit(0);
            //break;;
            
            
        }
        
        
        cin >> znak;
        if (znak=='h')
        {
            help();
        }
        
        cin>> operacja;
        if (operacja=='o')
        {
            operacja_o();
        }
        
        else if (operacja=='c')
        {
            operacja_c();
        }
        
        
        plik.close();
        return 0;
    }