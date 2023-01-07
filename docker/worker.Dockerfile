FROM base_steganography:latest

USER root

COPY --chown=mpiuser authorized_keys /home/mpiuser/.ssh/authorized_keys
RUN chmod 644 /home/mpiuser/.ssh/authorized_keys
RUN echo "PubkeyAuthentication yes" >> /etc/ssh/sshd_config
RUN echo "PasswordAuthentication  no" >> /etc/ssh/sshd_config

COPY --chown=mpiuser ../id_rsa_docker /home/mpiuser/.ssh/
COPY --chown=mpiuser config /home/mpiuser/.ssh/
RUN chmod 600 /home/mpiuser/.ssh/config
RUN chmod 600 /home/mpiuser/.ssh/id_rsa_docker
RUN chown mpiuser /home/mpiuser/.ssh

USER mpiuser

WORKDIR /home/mpiuser
COPY --chown=mpiuser .. /home/mpiuser/steganography

WORKDIR /home/mpiuser/steganography
RUN cmake .
RUN make

USER root
#CMD ["/usr/sbin/sshd", "-D"]
#CMD ["/usr/sbin/sshd"]
CMD ["/bin/bash"]
