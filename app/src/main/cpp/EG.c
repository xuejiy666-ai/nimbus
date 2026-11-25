int ep_ent_order(ZipReader *z, int pass, int i)
{
    const char *nm = z->e[i].name;
    switch (pass) {
    case 0: return strncmp(nm, "lib/", 4) == 0 && ends_with_ci(nm, ".so");
    case 1: return strcmp(nm, "assets/Epic_Config") == 0;
    case 2: return strcmp(nm, "classes.dex") == 0;
    case 3: return strcmp(nm, "AndroidManifest.xml") == 0;
    case 4: return strcmp(nm, "resources.arsc") == 0;
    default: return ends_with_ci(nm, ".dex");
    }
}
