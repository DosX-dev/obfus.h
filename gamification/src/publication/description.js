import { LEVEL_LABELS } from '../core/config.js';
const buttonAssets = 'https://raw.githubusercontent.com/DosX-dev/obfus.h/main/gamification/assets/download-buttons';
const escapeAttribute = value => String(value).replace(/[&<>"']/g, character => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[character]));
export function description(state) {
    const links = Object.entries(LEVEL_LABELS)
        .filter(([level]) => state.files.some(file => file.role === level))
        .map(([level, label]) => {
            const file = state.files.find(file => file.role === level);
            return `<a href="${escapeAttribute(file.url)}"><img src="${buttonAssets}/${level}.svg" alt="Download ${label}" width="240" height="120"></a>`;
        }).join('&#8201;\n');
    const answers = state.previous ? `\n\n<details>\n<summary>Previous round answers · Round ${state.previous.round}</summary>\n\n${Object.entries(state.previous.answers).map(([level, password]) => `- **${LEVEL_LABELS[level]}**: \`${password}\``).join('\n')}\n\n</details>` : '';
    const source = /^[a-f0-9]{40,64}$/.test(state.commit || '') ? ` · [Build source](https://github.com/DosX-dev/obfus.h/tree/${state.commit})` : '';
    return `Round **${state.round}** · **${state.date}** · Windows x64${source}\n\n<h2 align="center">Download task</h2>\n\n<p align="center">\n${links}\n</p>\n\nRecover the password. Patching does not count.\n\nNew round daily at **13:00 Moscow time (UTC+3)**. Save your downloads.${answers}\n`;
}
