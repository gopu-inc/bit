# 🛡️ Politique de Sécurité - Gopu Inc

> **"Security is not a feature, it's a state of mind."**

Chez **Gopu Inc**, nous prenons la sécurité de `bit` et de vos données Bitbucket extrêmement au sérieux. Ce document définit notre politique de gestion des vulnérabilités et les bonnes pratiques pour sécuriser votre environnement.

---

## 📦 Versions Supportées

Nous assurons les correctifs de sécurité uniquement pour la **dernière version stable** et la version majeure précédente.

| Version | Statut | Mises à jour de sécurité |
| :--- | :--- | :--- |
| **v1.0.x** | ✅ **Supporté** | Critique & Faible |
| **dev (main)** | ⚠️ **Instable** | Au fil de l'eau |
| < v1.0 | ❌ Fin de vie | Aucune |

---

## 🚨 Signaler une Vulnérabilité

Si vous découvrez une faille de sécurité dans `bit` (fuite de mémoire, exposition de token, injection de commande), **NE CRÉEZ PAS D'ISSUE PUBLIQUE**.

Nous pratiquons la **Divulgation Responsable (Responsible Disclosure)**.

### Procédure de signalement

1.  **Contact Direct** : Envoyez un email à `security@gopu-inc.com` (ou via Message Privé aux Administrateurs sur notre [Discord](https://discord.gg/RgJRE62AJ)).
2.  **Détails** : Incluez un "Proof of Concept" (PoC) ou les étapes pour reproduire la faille.
3.  **Chiffrement** : Si le rapport contient des informations sensibles, demandez notre clé PGP publique avant l'envoi.

### Notre Engagement (SLA)

*   **Réponse** : Sous 24 heures ouvrées.
*   **Analyse** : Validation de la faille sous 48 heures.
*   **Correctif** : Déploiement d'un patch critique sous 3 à 5 jours selon la complexité.

---

## 🔐 Gestion des Identifiants (Tokens)

`bit` manipule des tokens OAuth2 et des secrets clients Bitbucket. Voici comment nous les protégeons et ce que vous devez faire :

### Ce que fait `bit` :
*   Stockage local dans `~/.bitup/config`.
*   Permissions strictes appliquées automatiquement (`chmod 600`) : seul votre utilisateur système peut lire ce fichier.
*   Aucun envoi de télémétrie contenant vos secrets.

### Ce que vous devez faire :
*   Ne jamais commiter votre fichier `.bit/config` ou `~/.bitup/config`.
*   Utiliser des "App Passwords" Bitbucket avec les permissions minimales requises (Scope).
*   Si vous soupçonnez une compromission, révoquez immédiatement vos tokens sur Bitbucket et lancez `bit logout`.

---

## 🏆 Bug Bounty & Hall of Fame

Bien que nous n'ayons pas encore de programme de récompense financière, nous croyons en la reconnaissance.

Si vous nous signalez une faille critique de manière responsable :
1.  Vous serez crédité publiquement dans notre fichier `AUTHORS` et dans les Release Notes du correctif.
2.  Vous recevrez un rôle exclusif **"White Hat"** sur notre serveur Discord Gopu Inc.

---

<div align="center">
  <small>© 2026 Gopu Inc Security Team.</small>
</div>
