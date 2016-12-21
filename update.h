void doUpdate(){
    #ifdef DEBUG    
    serverClient.println((String("[HTTP] url: ") + updateurl + ", MD5: " + md5value + ", Fingerprint: " + fingerprint));
    #endif
    t_httpUpdate_return ret = ESPhttpUpdate.update(updateurl, md5value, fingerprint);
}
