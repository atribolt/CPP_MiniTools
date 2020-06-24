#ifndef PROPERTIES_PREPROCESSOR
#define PROPERTIES_PREPROCESSOR

// type proppertie
#define TPROP(type, name) type _##name;                                                 \
                          public:                                                       \
                          type const& name() const { return _##name; }                  \
                          void name(type const& val) { _##name = val; }

// auto type propertie
#define ATPROP(name) _##name;                                                           \
                     public:                                                            \
                     auto const& name() const { return _##name; }                       \
                     void name(decltype(_##name) const& val) { _##name = val; }

// auto propertie and custom getter and setter
#define AT_PROP(name, getter, setter) _##name;\
                                      public: \
                                      auto const& name () const getter\
                                      void name (decltype(_##name) const& value) setter

#endif // PROPERTIES_PREPROCESSOR



/****************EXAMPLE*********************** 
class A {
   int ATPROP(Age);

   const char* AT_PROP(
        Name
      , { 
            return _Name;  
      }
      , { 
            _Name = value; 
            _Age++; 
      }
   );

public:
   TPROP(float, Time);
   

};

void Test() {
   A a;

   a.Age(5); // a.Age() == 5

   a.Name("Вася"); // a.Age() == 6  
}
************END*EXAMPLE************************/