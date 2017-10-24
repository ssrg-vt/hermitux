
static long double  powers [] = {
    1.e+1, 1.e+2, 1.e+4, 1.e+8, 1.e+16, 1.e+32, 1.e+64, 1.e+128, 1.e+256
};

/*
 *  So, die ist zum Zerlegen von Gleitkommazahlen am besten geeignet.
 *
 *  Die nichtnegative übergebende Gleitkommazahl number wird in einen
 *  Exponenten e und eine Mantisse m zerlegt mit:
 *
 *      1 <= m < 10
 *      number = m * 10^e
 *
 *  Die Mantisse wird in precision Dezimalstellen zerlegt, die nach digits
 *  geschrieben werden.  digits[0] ist die Vorkommastelle, digits [1 ...
 *  precision-1] die Nachkommastellen der Mantisse Zurückgeliefert wird der
 *  Exponent.
 *
 *  Für precision ist ein Wert von 0 erlaubt, Sinn machen allerdings erst
 *  Werte ab 1.
 */

int  __decompose_floatp ( long double number, 
                          unsigned char* digits, unsigned int precision );

int  __decompose_floatp ( long double number, 
                          unsigned char* digits, unsigned int precision )
{
    int     ret = 0;
    int     i;
    double  tmp;
    
    if ( number > 0.L ) {
    
    	// Exponent abtrennen
        if ( number >= 10.L ) {
            for ( i = sizeof(powers)/sizeof(*powers)-1; i >= 0; i--)
                if ( number >= powers [i] ) {
                    number /= powers [i];
                    ret += 1 << i;
                }
	} else if ( number < 1.L )
            for ( i = sizeof(powers)/sizeof(*powers)-1; i >= 0; i--)
               if ( number * powers [i] < 10.L ) {
                   number *= powers [i];
                   ret -= 1 << i;
                }

	// Runden (ohne Geradezahlregel => Bug)
        tmp = 5.;
	{
	  unsigned int j;
	  for ( j = 0; j < precision; j++ )
	      tmp *= 0.1;
	}
        
        number += tmp;
                
        // Dabei kann die Zahl in die nächste Dekade reinrutschen ...
        if ( number >= 10.L ) {
            number = 1.L;
            ret++;
        }
    }

    // Mantisse in ASCII konvertieren
    while ( precision-- ) {
        i          = (int) number;
        number     = (number - i) * 10.L;
        *digits++  = '0' + i;
    }
    
    // Exponent zurück
    return ret;
}


/*
 *  So, die ist zum Zerlegen von Festkommazahlen am besten geeignet.
 *
 *  Die nichtnegative übergebende Festkomma wird in einen Integeranteil und
 *  einen Bruchanteil zerlegt.
 *
 *  Der Bruchanteil wird in digits_frac[0...precision_frac-1] gespeichert,
 *  falls precision_frac != 0 ist.
 *
 *  Der Integeranteil wird ab digits_int + precision_int - 1 rückwrts
 *  geschrieben.  Zurückgeliefert wird ein Zeiger auf das erste Zeichen, das
 *  bei der Konvertierung != '0' ist (Ausnahme ist die 0.0 selbst). Zeichen
 *  zwischen digits_int und diesem Zeiger (exklusive des Zeichens unter dem
 *  Zeiger) sind unbestimmt.  Wünscht man dort Nullen oder Leerzeichen,
 *  sollte man mittels memset() dieses vorher initialsieren.
 */

char*  __decompose_fixp ( long double number, 
                          unsigned char* digits_int , unsigned int precision_int, 
                          unsigned char* digits_frac, unsigned int precision_frac );

char*  __decompose_fixp ( long double number, 
                          unsigned char* digits_int , unsigned int precision_int, 
                          unsigned char* digits_frac, unsigned int precision_frac )
{
    long long int  integer;
    double         tmp;
    int            i;
    
    // Runden (ohne Geradezahlregel => Bug)
    tmp = 0.5;
    {
      unsigned int j;
      for ( j = 0; j < precision_frac; j++ )
	  tmp *= 0.1;
    }
    
    number += tmp;
    
    integer = number;
    number -= integer;

    // Nachkommastellen
    while ( precision_frac-- ) {
        number  *= 10.L;
        i        = (int) number;
        number  -= i;
        *digits_frac++  
	         = '0' + i;
    }

    // Vorkommastellen
    while ( precision_int ) {
        i        = (int) (integer % 10);
        integer /= 10;
        digits_int [--precision_int] 
                 = '0' + i;
        if ( integer == 0 )
            break;
    }
    
    return digits_int + precision_int;
}


#if 0

#include <stdio.h>
#include <math.h>

long double  test [] = { 
    1, M_PI, 123, 123456789, 12345678901234567, 1e300, 0.00123456789, 1.234567890123456e-300, 0
};

int main ( void )
{
    int    i;
    int    j;
    int    k;
    char   buff1 [32];
    char   buff2 [32];
    char*  retp;
    int    ret;
    
    for ( i = 0; i < sizeof(test)/sizeof(*test); i++ ) {
        printf ("\n*** %30.20Lf ***\n\n", test[i] );
        
	for ( j = 0; j <= 20; j++ ) {
	    memset ( buff1, 0, sizeof(buff1) );
	    ret = __decompose_floatp ( test[i], buff1, j );
	    printf ( "floatp(%2u) = <%sE%+d>\n", j, buff1, ret );
	}
	for ( j = 0; j <= 20; j++ ) {
	    for ( k = 0; k <= 20; k++ ) {
	        memset ( buff1, 0, sizeof(buff1) );
	        memset ( buff2, 0, sizeof(buff2) );
	        retp = __decompose_fixp ( test[i], buff1, j, buff2, k );
 	        printf ( "fixp(%2u,%2u) = <%s.%s>\n", j, k, retp, buff2 );
 	    }
	}
	    
    }
    
    return 0;
}

#endif
