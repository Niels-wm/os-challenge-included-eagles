void reversehashing (struct Request request) {
    struct Packet packet1 = request.packet;
    int n;
    int sock = request.reply_socket;
    uint64_t answer = find(packet1.hash);

    if (answer == 0) {
      // Reverse the start, end and p:
      packet1.start = be64toh(packet1.start);
      packet1.end = be64toh(packet1.end);
      /* SHA 256 ALGO */

      uint8_t theHash[32];

      for (answer = packet1.start; answer <= packet1.end; answer++){

          bzero(theHash, 32);
          SHA256((const unsigned char *) &answer, 8, theHash);

          if (memcmp(theHash, packet1.hash, sizeof(theHash)) == 0) {
            insert(packet1.hash, answer);
            break;
          }
      }
    }


    /* Send */
    answer = htobe64(answer);
    n = write(sock, &answer ,8);
    close(sock);

    if(n < 0) {
        perror("ERROR writing to socket");
    }
}
