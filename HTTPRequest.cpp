
int main()
{
    std :: cout << find_uri("GET /search?platform=Windows&category=office") << std :: endl;
    if (isValidHTTPMethod(find_methode("GET /search?platform=Windows&category=office")))
        std :: cout << "coucou\n";
    else 
        std :: cout << "salut\n";
}